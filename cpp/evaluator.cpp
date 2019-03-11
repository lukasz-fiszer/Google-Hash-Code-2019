#ifndef EVALUATOR_CPP
#define EVALUATOR_CPP

#include <algorithm>
#include <unordered_map>
#include "slide.cpp"

class Evaluator
{
public:

	static int get_transition_score(Slide& slide1, Slide& slide2)
	{
		int intersection_size = set_intersection_size(slide1.tags, slide2.tags);
		int only_slide1_size = slide1.tags.size() - intersection_size;
		int only_slide2_size = slide2.tags.size() - intersection_size;
		return std::min({only_slide1_size, intersection_size, only_slide2_size});
	}

	static int get_transition_score(Slide& slide1, Photo& photo)
	{
		int intersection_size = set_intersection_size(slide1.tags, photo.tags);
		int only_slide1_size = slide1.tags.size() - intersection_size;
		int only_photo_size = photo.tags.size() - intersection_size;
		return std::min({only_slide1_size, intersection_size, only_photo_size});
	}

	static int get_transition_score(Slide& slide1, Photo& photo1, Photo& photo2)
	{
		std::vector<Photo> slide_photos = std::vector<Photo>(2);
		slide_photos[0] = photo1;
		slide_photos[1] = photo2;
		Slide slide2 = Slide(slide_photos);
		return get_transition_score(slide1, slide2);
	}

	static int get_slideshow_score(std::vector<Slide*>& slideshow)
	{
		int score = 0;
		for(int i = 0; i < slideshow.size() - 1; i++)
		{
			score += Evaluator::get_transition_score(*(slideshow[i]), *(slideshow[i+1]));
		}
		return score;
	}

	static int get_slideshow_score(std::vector<Slide>& slideshow)
	{
		int score = 0;
		for(int i = 0; i < slideshow.size() - 1; i++)
		{
			score += Evaluator::get_transition_score(slideshow[i], slideshow[i+1]);
		}
		return score;
	}

	static bool check(std::vector<Slide>& slideshow)
	{
		std::unordered_map<int, int> photo_occurences = std::unordered_map<int, int>();
		bool check = true;
		for(Slide& slide: slideshow)
		{
			if(slide.photos.size() == 1)
			{
				if(slide.photos[0].horizontal == false) {std::cout << "f "; check = false;}
			}
			else if(slide.photos.size() == 2)
			{
				if(slide.photos[0].horizontal || slide.photos[1].horizontal) {std::cout << "2f "; check = false;}
			}
			else
			{
				std::cout << "fs ";
				check = false;
			}
			for(Photo& photo: slide.photos)
			{
				if(photo_occurences.count(photo.id) == 0)
				{
					photo_occurences[photo.id] = 1;
				}
				else
				{
					photo_occurences[photo.id]++;
					std::cout << "pr ";
					check = false;
				}
			}
		}
		return check;
	}

	template <typename T>
	static int set_intersection_size(std::set<T>& set1, std::set<T>& set2)
	{
		return set_intersection(set1, set2).size();
	}

	template <typename T>
	static int set_difference_size(std::set<T>& set1, std::set<T>& set2)
	{
		return set1.size() - set_intersection_size(set1, set2);
	}

	template <typename T>
	static std::set<T> set_intersection(std::set<T>& set1, std::set<T>& set2)
	{
		std::set<T> intersection = std::set<T>();
		std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersection, intersection.begin()));
		return intersection;
	}

};

#endif