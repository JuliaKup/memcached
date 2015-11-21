#include "buffer.h"
#include "protocol.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

MC_COMMAND CommandName2Code(std::string& param) {
	if (param == "add") {
		return CMD_ADD;
	} else if (param == "get") {
		return CMD_GET;
	} else if (param == "delete") {
		return CMD_DELETE;
	} else if (param == "set") {
		return CMD_SET;
	} else {
		throw std::runtime_error("Invalid command name");
		return CMD_UNKNOWN;
	}
}

void McCommand::Deserialize(RBuffer* buffer) {
    std::string cmd = buffer->ReadField(' '); // читает поле до пробельного символа
    buffer->ReadChar();                       // Читает ' ', и сдвигает текущий символ
    command = CommandName2Code(cmd);
    if (command == CMD_GET) {
        std::string untilr = buffer->ReadField('\r');

        size_t beginkey = 0;
        size_t endkey = 0;

        while (endkey < untilr.length()) {
        	while (untilr[endkey] != ' ' && endkey != untilr.length()) ++endkey;
        	std::string key(untilr, beginkey, endkey - beginkey);
        	keys.push_back(key);
            ++endkey;
            beginkey = endkey;
        }
    } else if (command == CMD_SET || command == CMD_ADD) {
    	keys.push_back(buffer->ReadField(' '));
    	buffer->ReadChar();
    	flags = buffer->ReadUint32();
    	buffer->ReadChar();  
    	exp_time = (time_t) buffer->ReadUint32();
    	buffer->ReadChar();  
    	uint32_t n = buffer->ReadUint32();

        buffer->ReadCharCheck('\r');
        buffer->ReadCharCheck('\n');

        data.resize(n);
        data = buffer->ReadBytes(n);
    } else if (command == CMD_DELETE) {
        keys.push_back(buffer->ReadField('\r'));
    }

    buffer->ReadCharCheck('\r'); // бросает исключение, если текущий символ не равен '\r', и сдвигает позицию текущего символа
    buffer->ReadCharCheck('\n');
}

std::string ResultCode2String(MC_RESULT_CODE code) {
	switch (code) {
    case R_STORED:
        {
            return "STORED";
            break;
        }
    case R_NOT_STORED:
        {
            return "NOT_STORED";
            break;
        }
    case R_EXISTS:
        {
            return "EXISTS";
            break;
        }
    case R_NOT_FOUND:
        {
            return "NOT_FOUND";
            break;
        }
    case R_DELETED:
        {
            return "DELETED";
            break;
        }
    }
}

void McValue::Serialize(WBuffer* buffer) const {
	buffer->WriteField(key_ +" ");
	buffer->WriteUint32(flags_);
	buffer->WriteField(" ");
	buffer->WriteUint32(data_.size());
	buffer->WriteField("\r\n");
	buffer->WriteBytes(data_);
	buffer->WriteField("\r\n");
}

void McResult::Serialize(WBuffer* buffer) const {
	switch(type_) {
        case RT_ERROR:
            {
                buffer->WriteField("ERROR", ' ');
                buffer->WriteField(error_message_);
                break;
            }
        case RT_VALUE:
            {
                for (int ind = 0; ind < values_.size(); ++ind) {
                    buffer->WriteField("VALUE", ' ');
                	values_[ind].Serialize(buffer);
                }
                buffer->WriteField("END");
                break;
            }
            
        case RT_CODE:
            {
        	    buffer->WriteField(ResultCode2String(code_));
                break;
            }
    }
    buffer->WriteChar('\r');
    buffer->WriteChar('\n');
}