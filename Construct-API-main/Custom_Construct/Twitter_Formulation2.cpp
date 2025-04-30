#include "pch.h"
#include "Twitter_Formulation2.h"

TwitterFormulation2::Lynnette_User::Lynnette_User(Social_Media_with_followers* media, const Node& node) :
	Social_Media_with_followers::default_media_user(media, node),
	Social_Media_no_followers::default_media_user(media, node)
{
}

TwitterFormulation2& TwitterFormulation2::Lynnette_User::media() {
	TwitterFormulation2* temp = dynamic_cast<TwitterFormulation2*>(media_ptr);
	// if the media couldn't be up casted the desired class this assertion will be raised.
	// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
	assert(temp);
	return *temp;
}

float TwitterFormulation2::Lynnette_User::get_user_attributes(Graph<float>* agent_attribute_network) {
	float attribute_sum = 0;
	for (int i = 0; i < agent_attribute_network->col_size; i++) {
		//float curr_attribute = media().agent_attribute_network->examine(id, i);
		float curr_attribute = agent_attribute_network->examine(id, i);
		attribute_sum += curr_attribute;
	}

	return attribute_sum;
}

void TwitterFormulation2::Lynnette_User::repost(media_event* _event)
{
	float repost_changed = get_user_attributes(media().retweet_attribute_network);

	if (random().uniform() < atan_clamp(repost_changed)) {
		media().create_repost(id, _event);
	}

}

void TwitterFormulation2::Lynnette_User::quote(media_event* _event)
{
	float quote_changed = get_user_attributes(media().quotes_attribute_network);
	if (random().uniform() < atan_clamp(quote_changed)) {
		media().create_quote(id, _event);
	}
}

void TwitterFormulation2::Lynnette_User::reply(media_event* _event)
{
	float reply_changed = get_user_attributes(media().reply_attribute_network);
	if (random().uniform() < atan_clamp(reply_changed)) {
		media().create_quote(id, _event);
	}
}

float TwitterFormulation2::Lynnette_User::sigmoid(float x)
{
	return 1.0f / (1.0f + std::exp(-x));
}

float TwitterFormulation2::Lynnette_User::atan_clamp(float x)
{
	//supposed to add 0.5 too but its okay
	return atan(x) / PI;
}

int TwitterFormulation2::Lynnette_User::likes_increase(Social_Media_no_followers::media_event* me)
{
	float likes_changed = get_user_attributes(media().likes_attribute_network);
	if (random().uniform() < atan_clamp(likes_changed)) {
		return me->indexes[InteractionItem::item_keys::likes] + 1;
	}

	return me->indexes[InteractionItem::item_keys::likes];
}

void TwitterFormulation2::Lynnette_User::parse(Social_Media_no_followers::media_event* me)
{
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

void TwitterFormulation2::Lynnette_User::enrich_event(Social_Media_no_followers::media_event* me)
{
	// Something done at every event step
	//me->indexes[InteractionItem::item_keys::attributes] = get_user_attributes(media().likes_attribute_network);
}

TwitterFormulation2::TwitterFormulation2(const dynet::ParameterMap& parameters, Construct& construct) : Twitter_wf(parameters, construct),
Social_Media_no_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct),
Social_Media_with_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct)
{

	for (auto& node : agents) {
		users[node.index] = new Lynnette_User(this, node);
	}

}

Social_Media_no_followers::media_event* TwitterFormulation2::create_post(unsigned int knowledge, unsigned int id)
{
	auto post = Social_Media_with_followers::create_post(knowledge, id);
	post->indexes[InteractionItem::item_keys::likes] = 0;

	return post;
}

void TwitterFormulation2::setupNetwork()
{
	retweet_attribute_network = construct.graph_manager.load_required(attributes::graph_retweets, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	reply_attribute_network = construct.graph_manager.load_optional(attributes::graph_replies, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	quotes_attribute_network = construct.graph_manager.load_optional(attributes::graph_quotes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	likes_attribute_network = construct.graph_manager.load_optional(attributes::graph_likes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	retweet_output_network = construct.graph_manager.load_required(attributes::retweet_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	replies_output_network = construct.graph_manager.load_required(attributes::replies_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	quotes_output_network = construct.graph_manager.load_required(attributes::quotes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	likes_output_network = construct.graph_manager.load_required(attributes::likes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

}