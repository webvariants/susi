#include "susi/duktape/Context.h"

void Duktape::Context::registerFunction(std::string name, NativeFunction function){
	functionCaller.registerFunction(name,function);
}

void Duktape::Context::unregisterFunction(std::string name){
	functionCaller.unregisterFunction(name);
}

void Duktape::Context::evalFile(std::string file){
	if(file != ""){
		if (duk_peval_file(ctx, file.c_str()) != 0) {
			throw std::runtime_error{std::string{"failed executing js sourcefile: "}+duk_safe_to_string(ctx, -1)};
		}
	}
}
void Duktape::Context::evalSource(std::string source){
	if(duk_peval_lstring(ctx, source.c_str(), source.size()) != 0){
		throw std::runtime_error{std::string{"failed executing js source: "}+duk_safe_to_string(ctx, -1)};
	}
}

BSON::Value Duktape::Context::getArgument(duk_context *ctx, int idx){
	int args = duk_get_top(ctx);
	int index = -args+idx+1;
	const char * jsonEncoded = duk_json_encode(ctx,index);
	return BSON::Value::fromJSON(jsonEncoded);
}

int Duktape::Context::getArgumentCount(duk_context *ctx){
	int args = duk_get_top(ctx);
	return args-1;
}

Duktape::Context::Context(){
	ctx = duk_create_heap_default();
	if (!ctx) {
		throw std::runtime_error{"failed creating duktape context"};
	}
	duk_push_global_object(ctx);
	duk_push_c_function(ctx, nativeCall, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "__nativeCall");
}

Duktape::Context::Context(std::string filename) : Duktape::Context::Context() {
	evalFile(filename);
}

Duktape::Context::~Context() {
	duk_destroy_heap(ctx);
}
