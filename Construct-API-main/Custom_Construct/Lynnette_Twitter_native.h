#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"

using namespace attributes;

const double PI = 3.14159265358979323846;

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

		float get_user_attributes(Graph<float>* agent_attribute_network) {
			float attribute_sum = 0;
			for (int i = 0; i < agent_attribute_network->col_size; i++) {
				//float curr_attribute = media().agent_attribute_network->examine(id, i);
				float curr_attribute = agent_attribute_network->examine(id, i);
				attribute_sum += curr_attribute;
			}

			return attribute_sum;
		}

		// If you want to edit repost probabilities, edit this function; then remember to edit output info
		void repost(media_event* _event) override {
			float repost_changed = get_user_attributes(media().retweet_attribute_network);

			//TODO: FIND ATTRIBUTES
			//for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
			//}
			
			if (random().uniform() < atan_clamp(repost_changed)) {
				media().create_repost(id, _event);
			}

		}

		void quote(media_event* _event) override {
			float quote_changed = get_user_attributes(media().quotes_attribute_network);
			if (random().uniform() < atan_clamp(quote_changed)) {
				media().create_quote(id, _event);
			}
		}

		void reply(media_event* _event) override {
			float reply_changed = get_user_attributes(media().reply_attribute_network);
			if (random().uniform() < atan_clamp(reply_changed)) {
				media().create_quote(id, _event);
			}
		}

		float sigmoid(float x) {
			return 1.0f / (1.0f + std::exp(-x));
		}

		float atan_clamp(float x) {
			//supposed to add 0.5 too but its okay
			return atan(x) / PI;
		}

		// increase likes by the probability of the attributes additions
		int likes_increase(Social_Media_no_followers::media_event* me) {
			float likes_changed = get_user_attributes(media().likes_attribute_network);
			if (random().uniform() < atan_clamp(likes_changed)) {
				return me->indexes[InteractionItem::item_keys::likes] + 1;
			}

			return me->indexes[InteractionItem::item_keys::likes];
		}

		void parse(Social_Media_no_followers::media_event* me) override {
			if (me->type == Social_Media_no_followers::media_event::event_type::post) {

				me->indexes[InteractionItem::item_keys::likes] = likes_increase(me);

				// write to output network
				unsigned int num_likes = me->indexes[InteractionItem::item_keys::likes];

				unsigned int num_reposts = me->reposts.size();
				unsigned int num_quotes = me->quotes.size();
				unsigned int num_replies = me->quotes.size();

				auto post_author = me->user;
				auto current_time = media().construct.current_time;
				
				media().likes_output_network->at(post_author, current_time) = num_likes;

				media().retweet_output_network->at(post_author, current_time) = num_reposts;
				media().quotes_output_network->at(post_author, current_time) = num_quotes;
				media().replies_output_network->at(post_author, current_time) = num_replies;
			}
		}

		void enrich_event(Social_Media_no_followers::media_event* me) override {
			// Don't know what the use of this is for so just putting something silly
			me->indexes[InteractionItem::item_keys::attributes] = get_user_attributes(media().likes_attribute_network);
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

	// Set forgetfulness of post to expoential decline
	//void update_event_scores() override {
	//	for (auto& post : this->list_of_events) {
	//		post.score = post.child_size() * exp(post.time_stamp);
	//	}
	//}

	Social_Media_no_followers::media_event* create_post(unsigned int knowledge, unsigned int id) override {
		auto post = Social_Media_with_followers::create_post(knowledge, id);
		post->indexes[InteractionItem::item_keys::likes] = 0;

		return post;
	}

	Graph<float>* retweet_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_retweets, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* reply_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_replies, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* quotes_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_quotes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<float>* likes_attribute_network = construct.graph_manager.load_required(attributes::network_agent_attributes_likes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	Graph<unsigned int>* retweet_output_network = construct.graph_manager.load_required(attributes::retweet_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	Graph<unsigned int>* replies_output_network = construct.graph_manager.load_required(attributes::replies_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	Graph<unsigned int>* quotes_output_network = construct.graph_manager.load_required(attributes::quotes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	Graph<unsigned int>* likes_output_network = construct.graph_manager.load_required(attributes::likes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

};