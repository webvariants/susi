#include "webstack/OnionHttpServerComponent.h"
#include "webstack/MimeTable.h"


onion_connection_status Susi::Webstack::HttpServerComponent::assetsHandler(Onion::Request &req, Onion::Response &res){
	sessionHandling(req,res);
	std::string targetFile = _assetRoot+req.path();
	char *realPath = realpath(targetFile.c_str(),NULL);
	if(realPath == nullptr){
		res.setCode(404);
        res<<"not found: "+targetFile;
        return OCS_PROCESSED;
	}else{
		targetFile = realPath;
		free(realPath);
	}
	if(targetFile.find(_assetRoot) != 0){
		res.setCode(403);
		res << "do not try to do this again!";
		res << targetFile;
        return OCS_PROCESSED;
	}
	std::ifstream file (targetFile, std::ios::in|std::ios::binary|std::ios::ate);
	if (file.is_open()){
		std::streamsize size = file.tellg();
		char *memblock = new char[size];
		file.seekg (0, std::ios::beg);
		file.read (memblock, size);
		file.close();
		auto contentType = MimeTable::getContentType(targetFile);
		res.setHeader("content-type",contentType);
		res << std::string{memblock,(unsigned long)size};
		delete [] memblock;
	}
    return OCS_PROCESSED;
}