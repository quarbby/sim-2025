#include "pch.h"
#include "Lynnette_twitter.h"

Lynnette_Twitter::Lynnette_User::Lynnette_User(Social_Media_with_followers* media, const Node& node) :
	Social_Media_with_followers::default_media_user(media, node),
	Social_Media_no_followers::default_media_user(media, node)
{
}

Lynnette_Twitter& Lynnette_Twitter::Lynnette_User::media()
{
	Lynnette_Twitter* temp = dynamic_cast<Lynnette_Twitter*>(media_ptr);
	// if the media couldn't be up casted the desired class this assertion will be raised.
	// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
	assert(temp);
	return *temp;
}

float Lynnette_Twitter::Lynnette_User::get_user_attributes()
{
	float attribute_sum = 0;
	for (int i = 0; i < media().agent_attribute_network->col_size; i++) {
		float curr_attribute = media().agent_attribute_network->examine(id, i);
		attribute_sum += curr_attribute;
	}

	return attribute_sum;
}

int Lynnette_Twitter::Lynnette_User::retweet_increase(Social_Media_no_followers::media_event* me)
{
	float retweets_changed = get_user_attributes();
	if (random().uniform() < sigmoid(retweets_changed)) {
		return me->indexes[InteractionItem::item_keys::retweets] + 1;
	}

	return me->indexes[InteractionItem::item_keys::retweets];
}

int Lynnette_Twitter::Lynnette_User::quotes_increase(Social_Media_no_followers::media_event* me)
{
	float quotes_increase = get_user_attributes();
	if (random().uniform() < sigmoid(quotes_increase)) {
		return me->indexes[InteractionItem::item_keys::quotes] + 1;
	}

	return me->indexes[InteractionItem::item_keys::quotes];
}

int Lynnette_Twitter::Lynnette_User::replies_increase(Social_Media_no_followers::media_event* me)
{
	float replies_increase = get_user_attributes();
	if (random().uniform() < sigmoid(replies_increase)) {
		return me->indexes[InteractionItem::item_keys::reply] + 1;
	}

	return me->indexes[InteractionItem::item_keys::reply];
}

float Lynnette_Twitter::Lynnette_User::sigmoid(float x)
{
	return 1.0f / (1.0f + std::exp(-x));
}

int Lynnette_Twitter::Lynnette_User::likes_increase(Social_Media_no_followers::media_event* me)
{
	float likes_changed = get_user_attributes();
	if (random().uniform() < sigmoid(likes_changed)) {
		return me->indexes[InteractionItem::item_keys::likes] + 1;
	}

	return me->indexes[InteractionItem::item_keys::likes];
}

void Lynnette_Twitter::Lynnette_User::parse(Social_Media_no_followers::media_event* me)
{
	if (me->type == Social_Media_no_followers::media_event::event_type::post) {

		me->indexes[InteractionItem::item_keys::likes] = likes_increase(me);

		// write to output network
		unsigned int num_likes = me->indexes[InteractionItem::item_keys::likes];

		unsigned int num_reposts = me->indexes[InteractionItem::item_keys::retweets];
		unsigned int num_quotes = me->indexes[InteractionItem::item_keys::quotes];
		unsigned int num_replies = me->indexes[InteractionItem::item_keys::reply];

		auto post_author = me->user;
		auto current_time = media().construct.current_time;
		
		media().likes_output_network->at(post_author, current_time) = num_likes;
		media().retweet_output_network->at(post_author, current_time) = num_reposts;
		media().quotes_output_network->at(post_author, current_time) = num_quotes;
		media().replies_output_network->at(post_author, current_time) = num_replies;
	}
}

void Lynnette_Twitter::Lynnette_User::enrich_event(Social_Media_no_followers::media_event* me)
{
	me->indexes[InteractionItem::item_keys::attributes] = get_user_attributes();
}

Lynnette_Twitter::Lynnette_Twitter(const dynet::ParameterMap& parameters, Construct& construct) : Twitter_wf(parameters, construct),
Social_Media_no_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct),
Social_Media_with_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct)
{

	for (auto& node : agents)
	{
		users[node.index] = new Lynnette_User(this, node);
	}
}

void Lynnette_Twitter::update_event_scores()
{
	for (auto& post : this->list_of_events) {
		post.score = post.child_size() * exp(post.time_stamp);
	}
}

Social_Media_no_followers::media_event* Lynnette_Twitter::create_post(unsigned int knowledge, unsigned int id)
{
	auto post = Social_Media_with_followers::create_post(knowledge, id);
	post->indexes[InteractionItem::item_keys::likes] = 0;

	return post;
}

void Lynnette_Twitter::setupNetwork()
{
	agent_attribute_network = construct.graph_manager.load_required(attributes::graph_agent_attributes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	retweet_output_network = construct.graph_manager.load_required(attributes::retweet_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	replies_output_network = construct.graph_manager.load_required(attributes::replies_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	quotes_output_network = construct.graph_manager.load_required(attributes::quotes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	likes_output_network = construct.graph_manager.load_required(attributes::likes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);
}