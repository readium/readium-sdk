#include "readium.h"

#include "ePub3/container.h"
#include "ePub3/initialization.h"

namespace {
 
bool ReadiumInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
    // Initialize SDK
    ePub3::InitializeSdk();
    ePub3::PopulateFilterManager();
    return true;
}

// HandleMessage gets invoked when postMessage is called on the DOM element
// associated with this plugin instance.
void ReadiumInstance::HandleMessage(const pp::Var& var_message) {
    // Ignore the message if it is not a string.
    if (!var_message.is_string())
        return;

    //Open book
    ePub3::string title = "unknown";
    std::string epub_path = var_message.AsString();
    std::shared_ptr<ePub3::Container> container = nullptr;

    try {
        container = ePub3::Container::OpenContainer(epub_path);

        if (container == nullptr) {
            printf("Unable to open container");
            return;
        }

        std::shared_ptr<ePub3::Package> package = nullptr;
    
        try {
            package = container->DefaultPackage();

            if (package == nullptr) {
                printf("Unable to open package");
                return;
            }

            title = package->Title(false);
            printf("Epub title: %s\n", title.c_str());
            printf("Epub authors: %s\n", package->Authors(false).c_str());
        } catch(const std::invalid_argument& ex) {
            printf("EPub3.isEpub3Book(): failed to open package: %s\n", ex.what());
        }
    } catch (const std::invalid_argument& ex) {
        printf("EPub3.isEpub3Book(): failed to open container: %s\n", ex.what());
    }

    pp::Var var_reply(title.c_str());
    PPPostMessage(var_reply);
}

// This object is the global object representing this plugin library as long
// as it is loaded.
// Override CreateInstance to create your customized Instance object.
pp::Instance* ReadiumModule::CreateInstance(PP_Instance instance) {
    return new ReadiumInstance(instance);
}

}

namespace pp {

// Factory function for your specialization of the Module object.
Module* CreateModule() {
    return new ReadiumModule();
}

}  // namespace pp
