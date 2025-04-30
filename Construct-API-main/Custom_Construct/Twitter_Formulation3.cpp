#include "pch.h"
#include "Twitter_Formulation3.h"

Lynnette_Twitter::Lynnette_User::Lynnette_User(Social_Media_with_followers* media, const Node& node) :
	Social_Media_with_followers::default_media_user(media, node),
	Social_Media_no_followers::default_media_user(media, node)
{
}

Lynnette_Twitter& Lynnette_Twitter::Lynnette_User::media() {
	Lynnette_Twitter* temp = dynamic_cast<Lynnette_Twitter*>(media_ptr);
	// if the media couldn't be up casted the desired class this assertion will be raised.
	// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
	assert(temp);
	return *temp;
}

float Lynnette_Twitter::Lynnette_User::sigmoid(float x) {
	return 1.0f / (1.0f + std::exp(-x));
}

unsigned int Lynnette_Twitter::Lynnette_User::vector_to_binary_int(std::vector<int>& positions) {
	unsigned int result = 0;

	for (int index : positions) {
		// Set the bit at position 'index'
		if (index < 32) {  // Ensure that the index is within the valid range for a 32-bit integer
			result |= (1U << index);
		}
	}

	return result;
}

std::vector<int> Lynnette_Twitter::Lynnette_User::binary_int_to_vector(unsigned int bendEValue, unsigned int numBits) {
	std::vector<int> positions;

	for (int i = 0; i < numBits; i++) {
		if (bendEValue & 1) { // Extract the least significant bit
			positions.push_back(i);
		}
		bendEValue >>= 1;                // Shift the bits to the right
	}

	return positions;
}

bool Lynnette_Twitter::Lynnette_User::does_like(Social_Media_no_followers::media_event* me) 
{
	unsigned int numBits = media().bende_probabilities_network->col_size;

	// number of bende based on probabilities
	std::vector<int> bende_vector = binary_int_to_vector(me->indexes[InteractionItem::item_keys::attributes], numBits);

	float sum_corr_value = 0;
	for (int i : bende_vector)
	{
		float corr_value = media().likes_attribute_network->examine(id, i);
		sum_corr_value += corr_value;
	}

	float sigmoid_output = sigmoid(sum_corr_value);
	float random_number = media().random.uniform();
	if (random_number < sigmoid_output)
	{
		return true;
	}

	return false;
}

bool Lynnette_Twitter::Lynnette_User::does_quote(Social_Media_no_followers::media_event* me)
{
	return false;
}

bool Lynnette_Twitter::Lynnette_User::does_reply(Social_Media_no_followers::media_event* me)
{
	return false;
}

bool Lynnette_Twitter::Lynnette_User::does_retweet(Social_Media_no_followers::media_event* me)
{
	return false;
}

void Lynnette_Twitter::Lynnette_User::parse(Social_Media_no_followers::media_event* me) {
	bool isPost = (me->type == Social_Media_no_followers::media_event::event_type::post);

	if (isPost)
	{

		unsigned int numBits = media().bende_probabilities_network->col_size;
		std::vector<int> bende_vector = binary_int_to_vector(me->indexes[InteractionItem::item_keys::attributes], numBits);
		unsigned int postAuthor = me->user;

		if (does_like(me))
		{
			me->indexes[InteractionItem::item_keys::likes] += 1;

			// write to output network

			for (int i : bende_vector) {
				media().likes_output_network->at(postAuthor, i) += 1;
			}

		}

		if (does_quote(me))
		{
			// TO COMPLETE
		}

		if (does_reply(me))
		{
			// TO COMPLETE
		}

		if (does_retweet(me))
		{
			// TO COMPLETE
		}
	}

}

void Lynnette_Twitter::Lynnette_User::enrich_event(Social_Media_no_followers::media_event* me) {
	// do this to every event - get a bendE for it
	me->indexes[InteractionItem::item_keys::attributes] = get_bendE();
}

Lynnette_Twitter::Lynnette_Twitter(const dynet::ParameterMap& parameters, Construct& construct) : Twitter_wf(parameters, construct),
Social_Media_no_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct),
Social_Media_with_followers("Twitter", InteractionItem::item_keys::twitter_event, parameters, construct)
{

	for (auto& node : agents) {
		users[node.index] = new Lynnette_User(this, node);
	}
}