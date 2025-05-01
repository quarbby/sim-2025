#include "pch.h"
#include "TwitterFormulation2.h"

TwitterFormulation2::LynnetteUser::LynnetteUser(Social_Media_with_followers* media, const Node& node) :
	Social_Media_with_followers::default_media_user(media, node),
	Social_Media_no_followers::default_media_user(media, node)
{
}

TwitterFormulation2& TwitterFormulation2::LynnetteUser::media() {
	TwitterFormulation2* temp = dynamic_cast<TwitterFormulation2*>(media_ptr);
	// if the media couldn't be up casted the desired class this assertion will be raised.
	// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
	assert(temp);
	return *temp;
}

float TwitterFormulation2::LynnetteUser::getUserAttributes(Graph<float>* agentAttributeNetwork) {
	float attributeSum = 0;
	for (int i = 0; i < agentAttributeNetwork->col_size; i++) {
		//float curr_attribute = media().agent_attribute_network->examine(id, i);
		float currAttribute = agentAttributeNetwork->examine(id, i);
		attributeSum += currAttribute;
	}

	return attributeSum;
}

void TwitterFormulation2::LynnetteUser::repost(media_event* _event)
{
	float repostChanged = getUserAttributes(media().retweetAttributeNetwork);

	if (random().uniform() < atanClamp(repostChanged)) {
		media().create_repost(id, _event);
	}

}

void TwitterFormulation2::LynnetteUser::quote(media_event* _event)
{
	float quoteChanged = getUserAttributes(media().quotesAttributeNetwork);
	if (random().uniform() < atanClamp(quoteChanged)) {
		media().create_quote(id, _event);
	}
}

void TwitterFormulation2::LynnetteUser::reply(media_event* _event)
{
	float replyChanged = getUserAttributes(media().replyAttributeNetwork);
	if (random().uniform() < atanClamp(replyChanged)) {
		media().create_quote(id, _event);
	}
}

float TwitterFormulation2::LynnetteUser::sigmoidActivation(float x)
{
	return 1.0f / (1.0f + std::exp(-x));
}

float TwitterFormulation2::LynnetteUser::atanClamp(float x)
{
	//supposed to add 0.5 too but its okay
	return atan(x) / PI;
}

int TwitterFormulation2::LynnetteUser::likesIncrease(Social_Media_no_followers::media_event* me)
{
	float likes_changed = getUserAttributes(media().likesAttributeNetwork);
	if (random().uniform() < atanClamp(likes_changed)) {
		return me->indexes[InteractionItem::item_keys::likes] + 1;
	}

	return me->indexes[InteractionItem::item_keys::likes];
}

void TwitterFormulation2::LynnetteUser::parse(Social_Media_no_followers::media_event* me)
{
	if (me->type == Social_Media_no_followers::media_event::event_type::post) {

		me->indexes[InteractionItem::item_keys::likes] = likesIncrease(me);

		// write to output network
		unsigned int numLikes = me->indexes[InteractionItem::item_keys::likes];

		unsigned int numReposts = me->reposts.size();
		unsigned int numQuotes = me->quotes.size();
		unsigned int numReplies = me->quotes.size();

		auto post_author = me->user;
		auto current_time = media().construct.current_time;

		media().likesOutputNetwork->at(post_author, current_time) = numLikes;

		media().retweetOutputNetwork->at(post_author, current_time) = numReposts;
		media().quotesOutputNetwork->at(post_author, current_time) = numQuotes;
		media().repliesOutputNetwork->at(post_author, current_time) = numReplies;
	}
}

void TwitterFormulation2::LynnetteUser::enrich_event(Social_Media_no_followers::media_event* me)
{
	// Something done at every event step
	//me->indexes[InteractionItem::item_keys::attributes] = get_user_attributes(media().likes_attribute_network);
}

TwitterFormulation2::TwitterFormulation2(const dynet::ParameterMap& parameters, Construct& construct) : Twitter_wf(parameters, construct),
Social_Media_no_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct),
Social_Media_with_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct)
{

	for (auto& node : agents) {
		users[node.index] = new LynnetteUser(this, node);
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
	retweetAttributeNetwork = construct.graph_manager.load_required(attributes::graph_retweets, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	replyAttributeNetwork = construct.graph_manager.load_optional(attributes::graph_replies, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	quotesAttributeNetwork = construct.graph_manager.load_optional(attributes::graph_quotes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	likesAttributeNetwork = construct.graph_manager.load_optional(attributes::graph_likes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	retweetOutputNetwork = construct.graph_manager.load_required(attributes::retweet_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	repliesOutputNetwork = construct.graph_manager.load_required(attributes::replies_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	quotesOutputNetwork = construct.graph_manager.load_required(attributes::quotes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	likesOutputNetwork = construct.graph_manager.load_required(attributes::likes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

}