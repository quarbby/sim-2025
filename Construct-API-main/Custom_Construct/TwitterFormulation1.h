#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "attributes.h"

using namespace attributes;


struct Lynnette_Twitter : public Twitter_wf {
	struct Lynnette_User : public Social_Media_with_followers::default_media_user
	{
		Lynnette_User(Social_Media_with_followers* media, const Node& node);

		Lynnette_Twitter& media();

		float get_user_attributes();

		// If you want to edit repost probabilities, edit this function; then remember to edit output info
		int retweet_increase(Social_Media_no_followers::media_event* me);

		int quotes_increase(Social_Media_no_followers::media_event* me);

		int replies_increase(Social_Media_no_followers::media_event* me);

		float sigmoid(float x);

		// increase likes by the probability of the attributes additions
		int likes_increase(Social_Media_no_followers::media_event* me);

		void parse(Social_Media_no_followers::media_event* me) override;

		void enrich_event(Social_Media_no_followers::media_event* me) override;
	};


	Lynnette_Twitter(const dynet::ParameterMap& parameters, Construct& construct);

	// Set forgetfulness of post to expoential decline
	void update_event_scores() override;

	Social_Media_no_followers::media_event* create_post(unsigned int knowledge, unsigned int id) override;

	Graph<float>* agent_attribute_network = nullptr;
	Graph<unsigned int>* retweet_output_network = nullptr;
	Graph<unsigned int>* replies_output_network = nullptr;
	Graph<unsigned int>* quotes_output_network = nullptr;
	Graph<unsigned int>* likes_output_network = nullptr;

	void setupNetwork();

};