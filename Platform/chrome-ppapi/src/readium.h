#ifndef readium_h
#define readium_h

#include <algorithm>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

namespace  {

const char* const kMessageId = "id";
const char* const kMessageType = "type";
const char* const kMessageData = "data";

const char* const kPath = "path";
const char* const kContentPath = "contentPath";

const char* const kMetadataTitle = "title";
const char* const kMetadataAuthors = "authors";

// Container  
const char* const kMessageContainerReadStream = "container:readStream";
const char* const kMessageContainerReadMetadata = "container:readMetadata";

class ReadiumInstance : public pp::Instance {
public:
    explicit ReadiumInstance(PP_Instance instance) : pp::Instance(instance) {}

    virtual ~ReadiumInstance() {}

    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);

    void HandleMessage(const pp::Var& var);
    int ContainerReadStream(const pp::Var& input, pp::Var &output);
    int ContainerReadMetadata(const pp::Var& input, pp::Var &output);

    // Alias method PostMessage to avoid conflict with win32 PostMessage
    void PPPostMessage(const pp::Var& var_message) {
        pp::Instance::PostMessage(var_message);
    }
};

// This object is the global object representing this plugin library as long
// as it is loaded.
class ReadiumModule : public pp::Module {
public:
    explicit ReadiumModule() : pp::Module() {}
    virtual ~ReadiumModule() {}

    virtual pp::Instance* CreateInstance(PP_Instance instance); 
};

}

#endif