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

		static bool headers_written = false;

		if (!headers_written) {
			// Write CSV header row
			_output_file << "current_timestep,timestep_created,user,last_used,num_quotes,num_reply,"
				<< "num_retweets,parent_event,root_event,msg_id" << std::endl;

			_output_file << "current_timestep,msg_id,timestep_created,user,parent_event,root_event,last_used,num_quotes,num_reply,num_retweets,back_present,build_present,bridge_present,boost_present,excite_present,engage_present,explain_present,enhance_present,neutralize_present,negate_present,narrow_present,neglect_present,dismay_present,dismiss_present,distort_present,distract_present,sad_present,fear_present,anger_present,happy_present,disgust_present,surprise_present" << std::endl;

			headers_written = true;
		}

		_output_file << current_timestep << COMMA_END
			<< &msg << COMMA_END
			<< msg.time_stamp << COMMA_END
			<< msg.user << COMMA_END
			<< msg.parent_event << COMMA_END
			<< msg.root_event << COMMA_END
			<< msg.last_used << COMMA_END

			<< msg.indexes.find(InteractionItem::item_keys::quotes)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::reply)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::retweets)->second << COMMA_END

			<< msg.indexes.find(InteractionItem::item_keys::back)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::build)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::bridge)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::boost)->second << COMMA_END

			<< msg.indexes.find(InteractionItem::item_keys::excite)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::engage)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::explain)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::enhance)->second << COMMA_END

			<< msg.indexes.find(InteractionItem::item_keys::neutralize)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::negate)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::narrow)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::neglect)->second << COMMA_END

			<< msg.indexes.find(InteractionItem::item_keys::dismay)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::dismiss)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::distort)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::distract)->second << COMMA_END

			<< msg.indexes.find(InteractionItem::item_keys::sad)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::fear)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::anger)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::happy)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::disgust)->second << COMMA_END
			<< msg.indexes.find(InteractionItem::item_keys::surprise)->second << COMMA_END

			<< std::endl;

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

		if (file.size() <= 4 || ".csv" != file.substr(file.size() - 4, 4)) 
		{
			throw dynet::wrong_file_extension(output_file, ".csv");
		}

		if (construct.working_directory != "")
		{
			file = construct.working_directory + dynet::seperator() + file;
		}

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}

		output_frequency = std::stoi(params.get_parameter(frequency));
	}
};