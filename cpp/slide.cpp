#ifndef SLIDE_CPP
#define SLIDE_CPP

#include <set>
#include <vector>
#include <string>
#include "photo.cpp"

typedef std::vector<Photo> photos_vector;
typedef std::set<std::string> tags_set;


class Slide
{
public:
	photos_vector photos;
	tags_set tags;
	bool used;

	Slide(){}

	Slide(photos_vector& photos)
	{
		this->photos = photos;
		this->tags = get_tags(photos);
		this->used = false;
	}

	std::string to_string()
	{
		std::string str = "";
		for(Photo& photo: photos)
		{
			str += std::to_string(photo.id) + " ";
		}
		str.pop_back();
		return str;
	}

	std::vector<int> get_photo_ids()
	{
		std::vector<int> photo_ids = std::vector<int>(photos.size());
		for(int i = 0; i < photos.size(); i++)
		{
			photo_ids[i] = photos[i].id;
		}
		return photo_ids;
	}

protected:
	tags_set get_tags(photos_vector& photos)
	{
		tags_set all_tags = tags_set();
		for(Photo& photo: photos)
		{
			for(std::string tag: photo.tags)
			{
				all_tags.insert(tag);
			}
		}
		return all_tags;
	}
};

#endif