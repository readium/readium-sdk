#include "readium.h"

#include "ePub3/container.h"
#include "ePub3/package.h"
#include "ePub3/initialization.h"
#include "ePub3/utilities/byte_stream.h"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array_buffer.h"

namespace {
 
bool ReadiumInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
    // Initialize SDK
    ePub3::InitializeSdk();
    ePub3::PopulateFilterManager();
    return true;
}

int ReadiumInstance::ContainerReadMetadata(const pp::Var& input, pp::Var &output) {
    // Get epub path and epub content path
    if (!input.is_dictionary()) {
        LogToConsole(PP_LOGLEVEL_ERROR, "Invalid data");
        return -1;
    }

    pp::VarDictionary input_data(input);

    if (!input_data.HasKey(kPath)) {
        LogToConsole(PP_LOGLEVEL_ERROR, "Invalid data");
        return -2;
    }

    // Open container
    std::shared_ptr<ePub3::Container> container = nullptr;

    // Path of epub
    std::string path = input_data.Get(kPath).AsString();

    try {
        container = ePub3::Container::OpenContainer(path);

        if (container == nullptr) {
            LogToConsole(PP_LOGLEVEL_ERROR, "Unable to open container");
            return;
        }

        std::shared_ptr<ePub3::Package> package = nullptr;

        // Open package    
        try {
            package = container->DefaultPackage();

            if (package == nullptr) {
                LogToConsole(PP_LOGLEVEL_ERROR, "Unable to open package");
                return;
            }

            pp::VarDictionary metadata;
            metadata.Set(kMetadataTitle, pp::Var(package->Title().c_str()));
            metadata.Set(kMetadataAuthors, pp::Var(package->Authors().c_str()));
            output = metadata;
        } catch(const std::invalid_argument& ex) {
            return -3;
        }
    } catch (const std::invalid_argument& ex) {
        return -4;
    }
    
    return 0;
}

int ReadiumInstance::ContainerReadStream(const pp::Var& input, pp::Var &output) {
    // Get epub path and epub content path
    if (!input.is_dictionary()) {
        LogToConsole(PP_LOGLEVEL_ERROR, "Invalid data");
        return -1;
    }

    pp::VarDictionary input_data(input);

    if (!input_data.HasKey(kPath) || !input_data.HasKey(kContentPath)) {
        LogToConsole(PP_LOGLEVEL_ERROR, "Invalid data");
        return -2;
    }

    // Open container
    std::shared_ptr<ePub3::Container> container = nullptr;

    // Path of epub
    std::string path = input_data.Get(kPath).AsString();

    // Path of content to retrieve inside the epub
    std::string content_path = input_data.Get(kContentPath).AsString();

    try {
        container = ePub3::Container::OpenContainer(path);

        if (container == nullptr) {
            LogToConsole(PP_LOGLEVEL_ERROR, "Unable to open container");
            return;
        }

        std::shared_ptr<ePub3::Package> package = nullptr;

        // Open package    
        try {
            package = container->DefaultPackage();

            if (package == nullptr) {
                LogToConsole(PP_LOGLEVEL_ERROR, "Unable to open package");
                return;
            }

            std::unique_ptr<ePub3::ByteStream> byte_stream = container->ReadStreamAtPath(content_path);
            pp::VarArrayBuffer array_buffer(byte_stream->BytesAvailable());
            char* data = static_cast<char*>(array_buffer.Map());
            byte_stream->ReadBytes(data, byte_stream->BytesAvailable());
            array_buffer.Unmap();
            output = array_buffer;
        } catch(const std::invalid_argument& ex) {
            return -3;
        }
    } catch (const std::invalid_argument& ex) {
        return -4;
    }
    
    return 0;
}

// HandleMessage gets invoked when postMessage is called on the DOM element
// associated with this plugin instance.
void ReadiumInstance::HandleMessage(const pp::Var& var) {
    // Ignore the message if it is not a string.
    if (!var.is_dictionary()) {
        LogToConsole(PP_LOGLEVEL_ERROR, "Invalid message");
        return;
    }

    pp::VarDictionary message(var);

    if (!message.HasKey(kMessageId) || !message.HasKey(kMessageType) || !message.HasKey(kMessageData)) {
        LogToConsole(PP_LOGLEVEL_ERROR, "Invalid message type");
        return;
    }

    int result = 0;
    pp::Var output;

    if (message.Get(kMessageType).AsString() == kMessageContainerReadStream) {
        result = ContainerReadStream(message.Get(kMessageData), output);
        message.Set(kMessageData, output);
        PPPostMessage(message);
    } else if (message.Get(kMessageType).AsString() == kMessageContainerReadMetadata) {
        result = ContainerReadMetadata(message.Get(kMessageData), output);
        message.Set(kMessageData, output);
        PPPostMessage(message);
    }
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
