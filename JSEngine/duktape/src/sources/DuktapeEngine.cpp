#include "DuktapeEngine.h"

Duktape::JSEngine *Duktape::enginePtr;

void Duktape::JSEngine::start() {
	Duktape::enginePtr = this;
	
    init();

    if(sourceFile != ""){
        if (duk_peval_file(ctx, sourceFile.c_str()) != 0) {
            LOG(ERROR) << "processing usersource: " << duk_safe_to_string(ctx, -1);
        }
    }

    if (_interactive){
        LOG(DEBUG) << "entering interactive session";
        interactiveLoop();
    }

    LOG(DEBUG) << "started duktape engine and loaded "<<sourceFile;
}

void Duktape::JSEngine::init(){
    ctx = duk_create_heap_default();
    if (!ctx) {
        printf("Failed to create a Duktape heap.\n");
        exit(1);
    }
    duk_push_global_object(ctx);
    duk_push_c_function(ctx, js_registerConsumer, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_registerConsumer");
    duk_push_c_function(ctx, js_registerProcessor, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_registerProcessor");
    duk_push_c_function(ctx, js_unregister, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_unregister");
    duk_push_c_function(ctx, js_publish, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_publish");
    duk_push_c_function(ctx, js_ack, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "_ack");
    duk_push_c_function(ctx, js_log, 1 /*nargs*/);
    duk_put_prop_string(ctx, -2, "log");

    if(duk_peval_lstring(ctx, Duktape::susiJS.c_str(), Duktape::susiJS.size()) != 0){
        LOG(ERROR) << "processing susiJS: " << duk_safe_to_string(ctx, -1);
    }
}

void Duktape::JSEngine::stop(){
	unsubscribeAll();
}

duk_ret_t Duktape::JSEngine::js_registerConsumer(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    enginePtr->registerConsumer(val);
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Duktape::JSEngine::js_registerProcessor(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    enginePtr->registerProcessor(val);
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Duktape::JSEngine::js_publish(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    try{
    	enginePtr->publish(val);
    }catch(const std::exception & e){
    	LOG(DEBUG) << "publish failed: "<<e.what();
    	duk_push_false(ctx);
    	return 1;
    }
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Duktape::JSEngine::js_ack(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    if(val!=nullptr){
	    enginePtr->ack(val);
	    duk_push_true(ctx);
    }else{
    	duk_push_false(ctx);
    }
    return 1;
}

duk_ret_t Duktape::JSEngine::js_log(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    enginePtr->addOutput(val);
    duk_push_true(ctx);
    return 1;
}

duk_ret_t Duktape::JSEngine::js_unregister(duk_context *ctx) {
    size_t sz;
    const char *topic = duk_require_lstring(ctx, 0, &sz);
    enginePtr->unregister(topic);
    duk_push_true(ctx);
    return 1;
}

void Duktape::JSEngine::registerProcessor(std::string topic){
	registerIds[topic] = BaseComponent::subscribe(topic,[this,topic](Susi::Events::EventPtr event){
		std::lock_guard<std::mutex> lock{mutex};
		auto eventString = event->toString();
		pendingEvents[event->id] = std::move(event);
		duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processProcessorEvent");
        duk_push_string(ctx, eventString.c_str());
        duk_push_string(ctx, topic.c_str());
        if (duk_pcall(ctx, 2 /*nargs*/) != 0) {
            addOutput(std::string{"Error: "}+duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);  /* pop result/error */
	});
}

void Duktape::JSEngine::unregister(std::string topic){
    BaseComponent::unsubscribe(registerIds[topic]);
    registerIds.erase(topic);
}

void Duktape::JSEngine::registerConsumer(std::string topic){
	Susi::Events::Consumer consumer = [this,topic](Susi::Events::SharedEventPtr event){
		std::lock_guard<std::mutex> lock{mutex};
		duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processConsumerEvent");
        duk_push_string(ctx, event->toString().c_str());
        duk_push_string(ctx, topic.c_str());
        if (duk_pcall(ctx, 2 /*nargs*/) != 0) {
            addOutput(std::string{"Error: "}+duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);  /* pop result/error */
	};
	registerIds[topic] = BaseComponent::subscribe(topic,consumer);
}

void Duktape::JSEngine::publish(std::string eventData){
	auto any = Susi::Util::Any::fromJSONString(eventData);
	Susi::Events::Event rawEvent{any};
	auto event = createEvent(rawEvent.topic);
	*event = rawEvent;
	BaseComponent::publish(std::move(event),[this](Susi::Events::SharedEventPtr event){
		std::lock_guard<std::mutex> lock{mutex};
		duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1 /*index*/, "_processAck");
        duk_push_string(ctx, event->toString().c_str());
        if (duk_pcall(ctx, 1 /*nargs*/) != 0) {
            addOutput(std::string{"Error: "}+duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);  /* pop result/error */	
	});
}

void Duktape::JSEngine::ack(std::string eventData){
	auto any = Susi::Util::Any::fromJSONString(eventData);
	Susi::Events::Event rawEvent{any};
	try{
		auto & pendingEvent = pendingEvents.at(rawEvent.id);
		*pendingEvent = rawEvent;
		pendingEvents.erase(rawEvent.id);
	}catch(...){}
}

void Duktape::JSEngine::addOutput(std::string line){
    output.push_back(line);
    int row,col;
    getmaxyx(stdscr,row,col);
    while(output.size()>row){
        output.pop_front();
    }
    if(currentMode == OUTPUT){
        printOutput();
    }
}

void Duktape::JSEngine::printOutput(){
    clear();
    for(auto currentLine : output){
        printw("%s\n",currentLine.c_str());
    }
    refresh();
}

void Duktape::JSEngine::interactiveLoop(){
    while(true){
        char ch = getch();
        switch(ch){
            case 27: //escape
            case 3: { // ctrl-c
                endwin();
                exit(0);
                break;
            }
            case 9: { // ctrl-i
                currentMode = INPUT;
                editCommands();
                currentMode = OUTPUT;
                printOutput();
                break;
            }
            case 15: { //ctrl-o
                currentMode = OUTPUT;
                printOutput();
                break;
            }
            default: {
                /*std::cout<<(int)ch<<" pressed"<<std::endl;*/
            }
        }
    }
    std::cout<<"while end"<<std::endl;
    endwin();
}


void Duktape::JSEngine::editCommands(){
    system((std::string{"nano "}+commandFile).c_str());
    std::ifstream t(commandFile);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::vector<std::string> lines;
    std::string item;
    while (std::getline(buffer, item, '\n')) {
        lines.push_back(item);
    }
    int pos;
    for(pos=0; pos<commands.size() && pos<lines.size(); ++pos){
        if(commands[pos] != lines[pos]){
            break;
        }
    }
    if(pos<lines.size()){
        std::string command;
        for(;pos<lines.size();++pos){
            command += lines[pos]+"\n";
        }
        if(duk_peval_lstring(ctx, command.c_str(), command.size()) != 0){
            LOG(ERROR) << "processing command: " << duk_safe_to_string(ctx, -1);
            addOutput(std::string{"processing usersource: "} + duk_safe_to_string(ctx, -1));
        }
    }
    commands = lines;
}