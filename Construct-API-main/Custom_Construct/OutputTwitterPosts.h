#pragma once
#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include <cmath>

#include "item_keys.h"
#include "attributes.h"
#include "TwitterFormulation3.h"

const std::string COMMA_END = ", ";

struct OutputTwitterPosts : public Output {
	std::ofstream _output_file;
	const TwitterFormulation3::event_container* media_events;
	int output_frequency;

	virtual const TwitterFormulation3::event_container* get_event_list(const dynet::ParameterMap& params, Construct& construct) {
		auto media_ptr = dynamic_cast<TwitterFormulation3*>(construct.model_manager.get_model(model_names::TWIT_wf));
		return &media_ptr->list_of_events;
	}

	~OutputTwitterPosts(void) {
		_output_file.flush();
		_output_file.close();
	}

	void msg_out(const Social_Media_no_followers::media_event& msg, const unsigned int current_timestep) {

		//TODO: Add BENDe

		static constexpr const char* tabs = "\t\t\t";
		_output_file << "\"current timestep\", " << current_timestep << COMMA_END;
		_output_file << "\"timestep created\", " << msg.time_stamp << COMMA_END;
		_output_file << "\"user\", \"" << msg.user << "\"" << COMMA_END;
		_output_file << "\"last used\", " << msg.last_used << COMMA_END;
		_output_file << "\"num_quotes\", " << msg.indexes.find(InteractionItem::item_keys::quotes)->second << COMMA_END;
		_output_file << "\"num_reply\", " << msg.indexes.find(InteractionItem::item_keys::reply)->second << COMMA_END;
		_output_file << "\"num_retweets\", " << msg.indexes.find(InteractionItem::item_keys::retweets)->second << COMMA_END;

		_output_file << "\"parent_event\", " << msg.parent_event << COMMA_END;
		_output_file << "\"root_event\", " << msg.root_event << COMMA_END;
		_output_file << "\"id\", " << &msg << std::endl;

	}

	void process(unsigned int t) {

		if ((t + 1) % output_frequency == 0) {
			for (auto& msg : *media_events) {
				msg_out(msg, t);
			}
		}
	}

	OutputTwitterPosts(const dynet::ParameterMap& params, Construct& construct) {

		//name of required parameters
		const char* output_file = "output file";
		const char* frequency = "frequency";

		media_events = get_event_list(params, construct);

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 4 || ".csv" != file.substr(file.size() - 4, 4)) {
			throw dynet::wrong_file_extension(output_file, ".csv");
		}

		if (construct.working_directory != "") file = construct.working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}

		output_frequency = std::stoi(params.get_parameter(frequency));
	}
};