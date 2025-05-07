#include "pch.h"
#include "TwitterFormulation3.h"

TwitterFormulation3::Lynnette_User::Lynnette_User(Social_Media_with_followers* media, const Node& node) :
	Social_Media_with_followers::default_media_user(media, node),
	Social_Media_no_followers::default_media_user(media, node)
{
	m_Media = media;
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
	std::vector<int> bende_vector = binaryIntToVector(me->indexes[InteractionItem::item_keys::bendE], numBits);

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
	int currAgentID = id;

	for (int i : bendEVector)
	{
		float corr_value = attributeNetwork->examine(currAgentID, i);
		sumCorrValue += corr_value;
	}

	return sumCorrValue;
}

bool TwitterFormulation3::Lynnette_User::does_quote(Social_Media_no_followers::media_event* me, std::vector<int> bende_vector)
{
	float sum_corr_value = getSumCorrValue(bende_vector, media().quotes_attribute_network);
	bool isQuote = compareWithRandom(sum_corr_value);

	return isQuote;
}

bool TwitterFormulation3::Lynnette_User::does_reply(Social_Media_no_followers::media_event* me, std::vector<int> bende_vector)
{
	float sum_corr_value = getSumCorrValue(bende_vector, media().reply_attribute_network);
	bool isReply = compareWithRandom(sum_corr_value);

	return isReply;
}

bool TwitterFormulation3::Lynnette_User::does_retweet(Social_Media_no_followers::media_event* me, std::vector<int> bende_vector)
{
	float sum_corr_value = getSumCorrValue(bende_vector, media().retweet_attribute_network);
	bool isRetweet = compareWithRandom(sum_corr_value);

	return isRetweet;
}

void TwitterFormulation3::Lynnette_User::writeToOutputNetwork(std::vector<int> bendEVector, int postAuthor, Graph<unsigned int>* engagementReceivedNetwork, Graph<unsigned int>* engagementGivenNetwork)
{
	// TODO: Figure out which is correct

	// Stephen wrote this 
	//for (int i : bendEVector) 
	//{
	//	outputNetwork->at(postAuthor, i) += 1;
	//}

	// add 1 at (postAuthor, currentTime)

	// I wrote this 
	int currTime = m_Media->time;
	int frequencyInt = std::stoi(attributes::output_posts_frequency);

	int currAuthorID = id;

	// Write to output network every n timestep
	if (currTime % frequencyInt == 0.0)
	{
		engagementReceivedNetwork->at(postAuthor, currTime) += 1;

		engagementGivenNetwork->at(currAuthorID, currTime) += 1;
	}


}

float TwitterFormulation3::Lynnette_User::getEngagementProb(int engagementType)
{
	float engagementProb = sigmoidActivation(media().engagement_probabilities_network->examine(id, engagementType));

	if (engagementProb == 0)
	{
		engagementProb = media().random.uniform();
	}

	return engagementProb;

}

std::pair<float, std::string> TwitterFormulation3::Lynnette_User::findHighestProbability()
{
	float quote_prob = getEngagementProb(bendEmotions::engagement_isQuote);
	float reply_prob = getEngagementProb(bendEmotions::engagement_isReply);
	float retweet_prob = getEngagementProb(bendEmotions::engagement_isRetweet);

	// looks like highest value now is always quotes - randomise it? 

	std::array<std::pair<float, std::string>, 3> probs = 
	{ 
		{
			{quote_prob, bendEmotions::itemkey_quotes},
			{reply_prob, bendEmotions::itemkey_reply},
			{retweet_prob, bendEmotions::itemkey_retweets}
		} 
	};

	// Find the element with the maximum probability using explicit iterator type
	std::array<std::pair<float, std::string>, 3>::iterator max_it =
		std::max_element(probs.begin(), probs.end(),
		[](const std::pair<float, std::string>& a, const std::pair<float, std::string>& b) 
	{
		return a.first < b.first;
	});

	return *max_it;
}


void TwitterFormulation3::Lynnette_User::parse(Social_Media_no_followers::media_event* me) 
{

	bool isPost = (me->type == Social_Media_no_followers::media_event::event_type::post);

	if (isPost)
	{
		unsigned int postAuthor = me->user;
		int currAgentID = id;

		float randomNumber = media().random.uniform();

		unsigned int numBits = media().bende_probabilities_network->col_size;
		std::vector<int> bende_vector = binaryIntToVector(me->indexes[InteractionItem::item_keys::bendE], numBits);

		//Decide what to engage - depends on engagement network probability
		//Decide whether to engage - depends on the BENDe vector attached to each post
		//Does only one engagement

		std::pair<float, std::string> highestProb = findHighestProbability();
		float highestProbValue = highestProb.first;
		std::string highestProbEngagement = highestProb.second;

		if (highestProbValue < randomNumber)
		{
			if ((highestProbEngagement == bendEmotions::itemkey_quotes) && (does_quote(me, bende_vector)))
			{
				me->indexes[InteractionItem::item_keys::quotes] += 1;
				writeToOutputNetwork(bende_vector, postAuthor, media().quotes_received_network, media().quotes_given_network);
			}

			if ((highestProbEngagement == bendEmotions::itemkey_reply) && (does_reply(me, bende_vector)))
			{
				me->indexes[InteractionItem::item_keys::reply] += 1;
				writeToOutputNetwork(bende_vector, postAuthor, media().replies_received_network, media().replies_given_network);
			}

			if ((highestProbEngagement == bendEmotions::itemkey_retweets) && (does_retweet(me, bende_vector)))
			{
				me->indexes[InteractionItem::item_keys::retweets] += 1;
				writeToOutputNetwork(bende_vector, postAuthor, media().retweet_received_network, media().retweet_given_network);
			}
		}

	}

}

// do these actions to every media event
void TwitterFormulation3::Lynnette_User::enrich_event(Social_Media_no_followers::media_event* me)
{
	// get a BENDe value from probabilities of each post
	std::vector<int> bendE_probs = get_bendE();
	unsigned int binary_rep = vector_to_binary_int(bendE_probs);

	me->indexes[InteractionItem::item_keys::bendE] = binary_rep;

	// add presence of each BENDe to post
	// Note: bendESet starts from 0, itemkeys starts from higher index

	std::unordered_set<int> bendESet(bendE_probs.begin(), bendE_probs.end());
	int bendEBase = (int)InteractionItem::item_keys::BendE_start + 1;

	for (int i = bendEBase; i < (int)InteractionItem::item_keys::BendE_end; ++i)
	{
		int relative_index = i - bendEBase;

		if (bendESet.count(relative_index)) {
			me->indexes[(InteractionItem::item_keys)i] = 1;
		}
		else {
			me->indexes[(InteractionItem::item_keys)i] = 0;
		}
	}

}

//get person's probability of bendE from matrix
//decide for each bende whether it will be present or not in the post by comparison with random number
// row = agent; id is getting from the class it inherits from
std::vector<int> TwitterFormulation3::Lynnette_User::get_bendE()
{
	std::vector<int> bendE_probs;

	int currAgentID = id;

	for (int i = 0; i < media().bende_probabilities_network->col_size; i++) {
		float curr_prob = media().bende_probabilities_network->examine(currAgentID, i);
		if (curr_prob == 0)
		{
			curr_prob = media().random.uniform();
		}

		float random_number = media().random.uniform();

		if (curr_prob > random_number) 
		{
			// stores the index of the activated bendE
			bendE_probs.push_back(i);
		}
	}

	return bendE_probs;
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

Social_Media_no_followers::media_event* TwitterFormulation3::create_post(unsigned int knowledgeIndex, unsigned int agentIndex) 
{
	Social_Media_with_followers::media_event* post = Social_Media_with_followers::create_post(knowledgeIndex, agentIndex);

	for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
		post->indexes[(InteractionItem::item_keys)i] = 0;
	}


	return post;
}

Social_Media_no_followers::media_event* TwitterFormulation3::create_response(unsigned int id, Social_Media_no_followers::media_event* parentId)
{
	Social_Media_with_followers::media_event* post = Social_Media_with_followers::create_response(id, parentId);

	for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
		post->indexes[(InteractionItem::item_keys)i] = 0;
	}

	return post;
}


void TwitterFormulation3::setupNetwork() 
{
	engagement_probabilities_network = construct.graph_manager.load_required(attributes::graph_engagement_probabilities, attributes::nodeset_graph_agent, attributes::nodeset_graph_engagement);

	bende_probabilities_network = construct.graph_manager.load_required(attributes::graph_bende_probabilities, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	retweet_attribute_network = construct.graph_manager.load_required(attributes::graph_retweets, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	reply_attribute_network = construct.graph_manager.load_required(attributes::graph_replies, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);

	quotes_attribute_network = construct.graph_manager.load_required(attributes::graph_quotes, attributes::nodeset_graph_agent, attributes::nodeset_graph_attributes);


	retweet_given_network = construct.graph_manager.load_required(attributes::retweet_given_network, attributes::nodeset_graph_agent, nodeset_names::time);

	replies_given_network = construct.graph_manager.load_required(attributes::replies_given_network, attributes::nodeset_graph_agent, nodeset_names::time);

	quotes_given_network = construct.graph_manager.load_required(attributes::quotes_given_network, attributes::nodeset_graph_agent, nodeset_names::time);


	retweet_received_network = construct.graph_manager.load_required(attributes::retweet_received_network, attributes::nodeset_graph_agent, nodeset_names::time);

	replies_received_network = construct.graph_manager.load_required(attributes::replies_received_network, attributes::nodeset_graph_agent, nodeset_names::time);

	quotes_received_network = construct.graph_manager.load_required(attributes::quotes_received_network, attributes::nodeset_graph_agent, nodeset_names::time);
}