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
		Lynnette_User(Social_Media_with_followers* media, const Node& node);

		Lynnette_Twitter& media();

		float get_user_attributes(Graph<float>* agent_attribute_network);

		// If you want to edit repost probabilities, edit this function; then remember to edit output info
		void repost(media_event* _event) override;

		void quote(media_event* _event) override;

		void reply(media_event* _event) override;

		float sigmoid(float x);

		float atan_clamp(float x);

		// increase likes by the probability of the attributes additions
		int likes_increase(Social_Media_no_followers::media_event* me);

		void parse(Social_Media_no_followers::media_event* me) override;

		void enrich_event(Social_Media_no_followers::media_event* me) override;
	};


	Lynnette_Twitter(const dynet::ParameterMap& parameters, Construct& construct);

	// Set forgetfulness of post to expoential decline
	//void update_event_scores() override {
	//	for (auto& post : this->list_of_events) {
	//		post.score = post.child_size() * exp(post.time_stamp);
	//	}
	//}

	Social_Media_no_followers::media_event* create_post(unsigned int knowledge, unsigned int id) override;

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