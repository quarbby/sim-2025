#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"

using namespace attributes;

struct TwitterFormulation3 : public Twitter_wf {
	struct Lynnette_User : public Social_Media_with_followers::default_media_user
	{
		Lynnette_User(Social_Media_with_followers* media, const Node& node);

		TwitterFormulation3& media();

		float sigmoid(float x);

		unsigned int vector_to_binary_int(std::vector<int>& positions);

		std::vector<int> binary_int_to_vector(unsigned int bendEValue, unsigned int numBits);

		std::vector<int> getBendEProbs(Social_Media_no_followers::media_event* me);

		float getSumCorrValue(std::vector<int> bendEVector, Graph<float> *attributeNetwork);

		void writeToOutputNetwork(std::vector<int> bendEVector, int postAuthor, Graph<unsigned int>* outputNetwork);

		bool compareWithRandom(float sumCorrValue);

		bool does_like(Social_Media_no_followers::media_event* me);

		bool does_quote(Social_Media_no_followers::media_event* me);

		bool does_reply(Social_Media_no_followers::media_event* me);

		bool does_retweet(Social_Media_no_followers::media_event* me);

		void parse(Social_Media_no_followers::media_event* me) override;

		//get person's probability of bendE from matrix
		// row = agent; id is getting from the class it inherits from
		unsigned int get_bendE() {
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

		void enrich_event(Social_Media_no_followers::media_event* me) override;
	};


	TwitterFormulation3(const dynet::ParameterMap& parameters, Construct& construct);

	Social_Media_no_followers::media_event* create_post(unsigned int knowledge, unsigned int id) override {
		auto post = Social_Media_with_followers::create_post(knowledge, id);

		for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
			post->indexes[(InteractionItem::item_keys)i] = 0;
		}

		return post;
	}

	Social_Media_no_followers::media_event* create_response(unsigned int id, Social_Media_no_followers::media_event* parentId) override {
		auto post = Social_Media_with_followers::create_response(id, parentId);

		for (int i = (int)InteractionItem::item_keys::Lynnette_start + 1; i < (int)InteractionItem::item_keys::Lynnette_end; i++) {
			post->indexes[(InteractionItem::item_keys)i] = 0;
		}

		return post;
	}

	Graph<float>* bende_probabilities_network = nullptr;
	Graph<float>* retweet_attribute_network = nullptr;
	Graph<float>* reply_attribute_network = nullptr;
	Graph<float>* quotes_attribute_network = nullptr;
	Graph<float>* likes_attribute_network = nullptr;
	Graph<unsigned int>* retweet_output_network = nullptr;
	Graph<unsigned int>* replies_output_network = nullptr;
	Graph<unsigned int>* quotes_output_network = nullptr;
	Graph<unsigned int>* likes_output_network = nullptr;

	void setupNetwork();

};