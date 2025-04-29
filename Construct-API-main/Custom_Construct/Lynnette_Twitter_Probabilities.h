#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"

using namespace attributes;

struct Lynnette_Twitter : public Twitter_wf {
	struct Lynnette_User : public Social_Media_with_followers::default_media_user
	{
		Lynnette_User(Social_Media_with_followers* media, const Node& node) :
			Social_Media_with_followers::default_media_user(media, node),
			Social_Media_no_followers::default_media_user(media, node) {
		}

		Lynnette_Twitter& media() {
			Lynnette_Twitter* temp = dynamic_cast<Lynnette_Twitter*>(media_ptr);
			// if the media couldn't be up casted the desired class this assertion will be raised.
			// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
			assert(temp);
			return *temp;
		}

		float sigmoid(float x) {
			return 1.0f / (1.0f + std::exp(-x));
		}

		//change to returning unsigned int
		unsigned int vector_to_binary_int(std::vector<int>& positions) {
			unsigned int result = 0;

			for (int index : positions) {
				// Set the bit at position 'index'
				if (index < 32) {  // Ensure that the index is within the valid range for a 32-bit integer
					result |= (1U << index);
				}
			}

			return result;
		}

		//change to unsigned int (instead of string) to vector
		std::vector<int> binary_int_to_vector(unsigned int bendEValue, unsigned int numBits) {
			std::vector<int> positions;

			for (int i = 0; i < numBits; i++) {
				if (bendEValue & 1) { // Extract the least significant bit
					positions.push_back(i);
				}
				bendEValue >>= 1;                // Shift the bits to the right
			}

			return positions;
		}


		bool does_like(Social_Media_no_followers::media_event* me) {
			//TODO: to complete
			unsigned int numBits = media().bende_probability_network->col_size;
		
			// number of bende based on probabilities
			std::vector<int> bend_bias_vector = binary_int_to_vector(me->indexes[InteractionItem::item_keys::attributes], numBits);

			return false;
		}

		bool does_quote(Social_Media_no_followers::media_event* me) {
			//TODO: to complete

			return false;
			//return true;
		}

		bool does_reply(Social_Media_no_followers::media_event* me) {
			//TODO: to complete

			return false;
			//return true;
		}

		bool does_retweet(Social_Media_no_followers::media_event* me) {
			//TODO: to complete

			return false;
			//return true;
		}

		void parse(Social_Media_no_followers::media_event* me) override {
			if (me->type == Social_Media_no_followers::media_event::event_type::post &&
				does_like(me)) {
				me->indexes[InteractionItem::item_keys::likes] += 1;
				
				//TO COMPLETE
			}
		}

		//get person's probability of bendE from matrix
		// row = agent; id is getting from the class it inherits from
		unsigned int get_bendE() {
			std::vector<int> bendE_probs;

			for (int i = 0; i < media().bende_probability_network->col_size; i++) {
				float curr_prob = media().bende_probability_network->examine(id, i);
				float random_number = media().random.uniform();

				if (curr_prob > random_number) {
					bendE_probs.push_back(i);
				}
			}

			unsigned int binary_rep = vector_to_binary_int(bendE_probs);

			return binary_rep;
		}

		void enrich_event(Social_Media_no_followers::media_event* me) override {
			// notsure what it does
			me->indexes[InteractionItem::item_keys::attributes] = get_bendE();
		}
	};


	Lynnette_Twitter(const dynet::ParameterMap& parameters, Construct& construct) : Twitter_wf(parameters, construct),
		Social_Media_no_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct),
		Social_Media_with_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct)
	{

		for (auto& node : agents) {
			users[node.index] = new Lynnette_User(this, node);
		}

	}

	// Set forgetfulness to expoential
	//void update_event_scores() override {
	//	for (auto& post : this->list_of_events) {
	//		post.score = post.child_size() * exp(post.time_stamp);
	//	}
	//}

	Social_Media_no_followers::media_event* create_post(unsigned int knowledge, unsigned int id) override {
		auto post = Social_Media_with_followers::create_post(knowledge, id);

		for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
			post->indexes[(InteractionItem::item_keys)i] = 0;
		}

		//post->indexes[InteractionItem::item_keys::likes] = 0;
		//post->indexes[InteractionItem::item_keys::retweets] = 0;
		//post->indexes[InteractionItem::item_keys::quotes] = 0;
		//post->indexes[InteractionItem::item_keys::reply] = 0;

		return post;
	}

	Social_Media_no_followers::media_event* create_response(unsigned int id, Social_Media_no_followers::media_event* parentId) override {
		auto post = Social_Media_with_followers::create_response(id, parentId);

		for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
			post->indexes[(InteractionItem::item_keys)i] = 0;
		}

		return post;
	}

	// Add required graphs
	Graph<float>* bende_probability_network = construct.graph_manager.load_required(attributes::network_agent_attributes_probabilities, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* retweet_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_retweets, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* reply_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_replies, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* quotes_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_quotes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* likes_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_likes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<unsigned int>* retweet_output_network = construct.graph_manager.load_required(attributes::retweet_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	Graph<unsigned int>* replies_output_network = construct.graph_manager.load_required(attributes::replies_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	Graph<unsigned int>* quotes_output_network = construct.graph_manager.load_required(attributes::quotes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	Graph<unsigned int>* likes_output_network = construct.graph_manager.load_required(attributes::likes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);
};
