#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"

#include <cmath>
#include <algorithm>
#include <array>
#include <utility>

#include "item_keys.h"
#include "attributes.h"
#include "bendEmotions.h"

using namespace attributes;
using namespace bendEmotions;

struct TwitterFormulation3 : public Twitter_wf {
	struct Lynnette_User : public Social_Media_with_followers::default_media_user
	{
		Lynnette_User(Social_Media_with_followers* media, const Node& node);

		TwitterFormulation3& media();

		float sigmoidActivation(float x);

		unsigned int vector_to_binary_int(std::vector<int>& positions);

		std::vector<int> binaryIntToVector(unsigned int bendEValue, unsigned int numBits);

		std::vector<int> getBendEProbs(Social_Media_no_followers::media_event* me);

		float getSumCorrValue(std::vector<int> bendEVector, Graph<float> *attributeNetwork);

		void writeToOutputNetwork(std::vector<int> bendEVector, int postAuthor, Graph<unsigned int>* engagementReceivedNetwork, Graph<unsigned int>* engagementGivenNetwork);

		bool compareWithRandom(float sumCorrValue);

		float getEngagementProb(int engagementType);

		std::pair<float, std::string> findHighestProbability();

		bool does_quote(Social_Media_no_followers::media_event* me, std::vector<int> bende_vector);

		bool does_reply(Social_Media_no_followers::media_event* me, std::vector<int> bende_vector);

		bool does_retweet(Social_Media_no_followers::media_event* me, std::vector<int> bende_vector);

		void parse(Social_Media_no_followers::media_event* me) override;

		std::vector<int> get_bendE();

		void enrich_event(Social_Media_no_followers::media_event* me) override;

		Social_Media_with_followers* m_Media = nullptr;
	};

	TwitterFormulation3(const dynet::ParameterMap& parameters, Construct& construct);

	Social_Media_no_followers::media_event* create_post(unsigned int knowledgeIndex, unsigned int agentIndex) override;

	Social_Media_no_followers::media_event* create_response(unsigned int id, Social_Media_no_followers::media_event* parentId) override;

	Graph<float>* engagement_probabilities_network = nullptr;
	Graph<float>* bende_probabilities_network = nullptr;
	Graph<float>* retweet_attribute_network = nullptr;
	Graph<float>* reply_attribute_network = nullptr;
	Graph<float>* quotes_attribute_network = nullptr;

	Graph<unsigned int>* retweet_given_network = nullptr;
	Graph<unsigned int>* replies_given_network = nullptr;
	Graph<unsigned int>* quotes_given_network = nullptr;

	Graph<unsigned int>* retweet_received_network = nullptr;
	Graph<unsigned int>* replies_received_network = nullptr;
	Graph<unsigned int>* quotes_received_network = nullptr;

	void setupNetwork();

};