#include "pch.h"
#include "TwitterFormulation3.h"

TwitterFormulation3::Lynnette_User::Lynnette_User(Social_Media_with_followers* media, const Node& node) :
	Social_Media_with_followers::default_media_user(media, node),
	Social_Media_no_followers::default_media_user(media, node)
{
}

TwitterFormulation3& TwitterFormulation3::Lynnette_User::media() 
{
	TwitterFormulation3* temp = dynamic_cast<TwitterFormulation3*>(media_ptr);
	// if the media couldn't be up casted the desired class this assertion will be raised.
	// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
	assert(temp);
	return *temp;
}

float TwitterFormulation3::Lynnette_User::sigmoidActivation(float x) 
{
	return 1.0f / (1.0f + std::exp(-x));
}

unsigned int TwitterFormulation3::Lynnette_User::vector_to_binary_int(std::vector<int>& positions) 
{
	unsigned int result = 0;

	for (int index : positions) {
		// Set the bit at position 'index'
		if (index < 32) {  // Ensure that the index is within the valid range for a 32-bit integer
			result |= (1U << index);
		}
	}

	return result;
}

std::vector<int> TwitterFormulation3::Lynnette_User::binaryIntToVector(unsigned int bendEValue, unsigned int numBits) 
{
	std::vector<int> positions;

	for (int i = 0; i < numBits; i++) {
		if (bendEValue & 1) { // Extract the least significant bit
			positions.push_back(i);
		}
		bendEValue >>= 1;                // Shift the bits to the right
	}

	return positions;
}

std::vector<int> TwitterFormulation3::Lynnette_User::getBendEProbs(Social_Media_no_followers::media_event* me)
{
	unsigned int numBits = media().bende_probabilities_network->col_size;

	// number of bende based on probabilities
	std::vector<int> bende_vector = binaryIntToVector(me->indexes[InteractionItem::item_keys::attributes], numBits);

	return bende_vector;
}

bool TwitterFormulation3::Lynnette_User::compareWithRandom(float sumCorrValue)
{
	float sigmoidOutput = sigmoidActivation(sumCorrValue);
	float randomNumber = media().random.uniform();
	if (randomNumber < sigmoidOutput)
	{
		return true;
	}

	return false;
}

float TwitterFormulation3::Lynnette_User::getSumCorrValue(std::vector<int> bendEVector, Graph<float> *attributeNetwork)
{
	float sumCorrValue = 0;
	for (int i : bendEVector)
	{
		float corr_value = attributeNetwork->examine(id, i);
		sumCorrValue += corr_value;
	}

	return sumCorrValue;
}

bool TwitterFormulation3::Lynnette_User::does_like(Social_Media_no_followers::media_event* me)
{
	std::vector<int> bende_vector = getBendEProbs(me);
	float sum_corr_value = getSumCorrValue(bende_vector, media().likes_attribute_network);
	bool isLike = compareWithRandom(sum_corr_value);

	return isLike;
}

bool TwitterFormulation3::Lynnette_User::does_quote(Social_Media_no_followers::media_event* me)
{
	std::vector<int> bende_vector = getBendEProbs(me);
	float sum_corr_value = getSumCorrValue(bende_vector, media().quotes_attribute_network);
	bool isQuote = compareWithRandom(sum_corr_value);

	return isQuote;
}

bool TwitterFormulation3::Lynnette_User::does_reply(Social_Media_no_followers::media_event* me)
{
	std::vector<int> bende_vector = getBendEProbs(me);
	float sum_corr_value = getSumCorrValue(bende_vector, media().reply_attribute_network);
	bool isReply = compareWithRandom(sum_corr_value);

	return isReply;
}

bool TwitterFormulation3::Lynnette_User::does_retweet(Social_Media_no_followers::media_event* me)
{
	std::vector<int> bende_vector = getBendEProbs(me);
	float sum_corr_value = getSumCorrValue(bende_vector, media().retweet_attribute_network);
	bool isRetweet = compareWithRandom(sum_corr_value);

	return isRetweet;
}

void TwitterFormulation3::Lynnette_User::writeToOutputNetwork(std::vector<int> bendEVector, int postAuthor, Graph<unsigned int>* outputNetwork)
{
	for (int i : bendEVector) {
		outputNetwork->at(postAuthor, i) += 1;
	}
}

void TwitterFormulation3::Lynnette_User::parse(Social_Media_no_followers::media_event* me) {
	bool isPost = (me->type == Social_Media_no_followers::media_event::event_type::post);

	if (isPost)
	{

		unsigned int numBits = media().bende_probabilities_network->col_size;
		std::vector<int> bende_vector = binaryIntToVector(me->indexes[InteractionItem::item_keys::attributes], numBits);
		unsigned int postAuthor = me->user;

		if (does_like(me))
		{
			me->indexes[InteractionItem::item_keys::likes] += 1;
			writeToOutputNetwork(bende_vector, postAuthor, media().likes_output_network);

		}

		if (does_quote(me))
		{
			me->indexes[InteractionItem::item_keys::quotes] += 1;
			writeToOutputNetwork(bende_vector, postAuthor, media().quotes_output_network);
		}

		if (does_reply(me))
		{
			me->indexes[InteractionItem::item_keys::reply] += 1;
			writeToOutputNetwork(bende_vector, postAuthor, media().replies_output_network);
		}

		if (does_retweet(me))
		{
			me->indexes[InteractionItem::item_keys::retweets] += 1;
			writeToOutputNetwork(bende_vector, postAuthor, media().retweet_output_network);
		}
	}

}

void TwitterFormulation3::Lynnette_User::enrich_event(Social_Media_no_followers::media_event* me) {
	// do this to every event - get a bendE for it
	me->indexes[InteractionItem::item_keys::attributes] = get_bendE();
}

//get person's probability of bendE from matrix
// row = agent; id is getting from the class it inherits from
unsigned int TwitterFormulation3::Lynnette_User::get_bendE() {
	std::vector<int> bendE_probs;

	for (int i = 0; i < media().bende_probabilities_network->col_size; i++) {
		float curr_prob = media().bende_probabilities_network->examine(id, i);
		float random_number = media().random.uniform();

		if (curr_prob > random_number) {
			bendE_probs.push_back(i);
		}
	}

	unsigned int binary_rep = vector_to_binary_int(bendE_probs);

	return binary_rep;
}

TwitterFormulation3::TwitterFormulation3(const dynet::ParameterMap& parameters, Construct& construct) : Twitter_wf(parameters, construct),
Social_Media_no_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct),
Social_Media_with_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct)
{

	for (auto& node : agents)
	{
		users[node.index] = new Lynnette_User(this, node);
	}
}

Social_Media_no_followers::media_event* TwitterFormulation3::create_post(unsigned int knowledge, unsigned int id) 
{
	auto post = Social_Media_with_followers::create_post(knowledge, id);

	for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
		post->indexes[(InteractionItem::item_keys)i] = 0;
	}

	return post;
}

Social_Media_no_followers::media_event* TwitterFormulation3::create_response(unsigned int id, Social_Media_no_followers::media_event* parentId)
{
	auto post = Social_Media_with_followers::create_response(id, parentId);

	for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
		post->indexes[(InteractionItem::item_keys)i] = 0;
	}

	return post;
}


void TwitterFormulation3::setupNetwork() 
{
	bende_probabilities_network = construct.graph_manager.load_required(attributes::graph_bende_probabilities, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	retweet_attribute_network = construct.graph_manager.load_required(attributes::graph_retweets, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	reply_attribute_network = construct.graph_manager.load_required(attributes::graph_replies, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	quotes_attribute_network = construct.graph_manager.load_required(attributes::graph_quotes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	likes_attribute_network = construct.graph_manager.load_required(attributes::graph_likes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	retweet_output_network = construct.graph_manager.load_required(attributes::retweet_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	replies_output_network = construct.graph_manager.load_required(attributes::replies_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	quotes_output_network = construct.graph_manager.load_required(attributes::quotes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);

	likes_output_network = construct.graph_manager.load_required(attributes::likes_output_network, attributes::nodeset_graph_agent, nodeset_names::time);
}