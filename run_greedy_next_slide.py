import operator
from functools import reduce
from random import shuffle, randint, uniform, random

class Photo:
	def __init__(self, id, horizontal, tags):
		self.id = id
		self.horizontal = horizontal
		self.tags = tags
		self.tags_count = len(tags)
		self.used = False

	def has_tag(self, tag):
		return tag in self.tags

class Slide:
	def __init__(self, photos):
		self.photos = photos
		self.tags = Slide.get_tags(photos)
		self.used = False

	def get_tags(photos):
		tags_set = set()
		for photo in photos:
			tags_set.update(photo.tags)
		return tags_set

	def to_string(self):
		return reduce(lambda sum, current: sum + ' ' + str(current), self.get_photo_ids(), '')[1:]

	def get_photo_ids(self):
		return tuple(map(lambda photo: photo.id, self.photos))

class Evaluator:
	def __init__(self, slides):
		self.slides = slides
		self.horizontal_count = sum(1 for slide in slides if slide.photos[0].horizontal)
		self.vertical_count = sum(1 for slide in slides if not slide.photos[0].horizontal)
		self.get_score()
		Evaluator.check_slides(slides)

	def get_score(self):
		if 'score' not in dir(self):
			self.score = self.generate_score()
		return self.score

	def generate_score(self):
		score = 0
		Evaluator.check_slides(self.slides)
		for i in range(len(self.slides)-1):
			score += Evaluator.get_transition_score(self.slides[i], self.slides[i+1])
		return score

	def get_transition_score(slide1, slide2):
		tags1 = slide1.tags
		tags2 = slide2.tags
		return min(len(tags1.difference(tags2)), len(tags1.intersection(tags2)), len(tags2.difference(tags1)))

	def check_slides(slides):
		used_ids = set()
		for slide in slides:
			Evaluator.check_slide(slide)
			for photo in slide.photos:
				if photo.id in used_ids:
					raise AssertionError('photo used more than once')
				used_ids.add(photo.id)

	def check_slide(slide):
		photos = slide.photos
		if len(photos) not in [1, 2]:
			raise AssertionError('Photos per slide 1 or 2')

		if len(photos) == 1:
			assert photos[0].horizontal == True
		if len(photos) == 2:
			assert photos[0].horizontal == False and photos[1].horizontal == False

	def print_out(self):
		print(len(self.slides))
		for slide in self.slides:
			print(slide.to_string())

	def incremental_run(self, tries_per_step = 10):
		step_count = 0
		while True:
			print_log('----------------------------------------------------')
			print_log('Step count ' + str(step_count))
			[tries, change, score_increase] = self.next_step(tries_per_step)
			print_log('Tries taken ' + str(tries))
			print_log(change)
			print_log('Score increase ' + str(score_increase))
			print_log('Current score ' + str(self.get_score()))
			if tries > tries_per_step:
				break
			step_count += 1

	def next_step(self, tries_per_step):
		for change_number in range(tries_per_step):
			[change, score_increase] = self.perform_change()
			if score_increase > 0:
				return [change_number, change, score_increase]
		return [tries_per_step+1, '', 0]

	def perform_change(self):
		slide_swap = random() < (self.horizontal_count / len(self.slides))
		if slide_swap:
			[score_change, index1, index2] = self.swap_slides()
			return ['swap slides %i %i' % (index1, index2), score_change]
		else:
			[score_change, id1, id2] = self.swap_photos()
			return ['swap photos %i %i' % (id1, id2), score_change]

	def swap_slides(self):
		slides_count = len(self.slides)
		index1 = randint(0, slides_count - 1)
		index2 = randint(0, slides_count - 1)

		# score part from these indexes that were randomly chosen
		score_part1 = self.score_from_slides(index1, index2)

		# swap slides
		self.slides[index1], self.slides[index2] = self.slides[index2], self.slides[index1]

		# score part after swap
		score_part2 = self.score_from_slides(index1, index2)

		# score_change = score_part2 - score_part
		score_change = score_part2 - score_part1

		# reswap if needed, if score did not rise
		if score_change <= 0:
			self.slides[index1], self.slides[index2] = self.slides[index2], self.slides[index1]
		else:
			self.score += score_change

		return [score_change, index1, index2]


	def swap_photos(self):
		slides_count = len(self.slides)
		index1 = randint(0, slides_count - 1)
		index2 = randint(0, slides_count - 1)

		photo1 = 0 if len(self.slides[index1].photos) == 1 else randint(0, 1)
		photo2 = 0 if len(self.slides[index2].photos) == 1 else randint(0, 1)

		photo1Object = self.slides[index1].photos[photo1]
		photo2Object = self.slides[index2].photos[photo2]

		# score from randomly chosen slides
		score_part1 = self.score_from_slides(index1, index2)
		
		# swap chosen photos
		self.slides[index1].photos[photo1] = photo2Object
		self.slides[index2].photos[photo2] = photo1Object

		# score from slides after swap
		score_part2 = self.score_from_slides(index1, index2)

		score_change = score_part2 - score_part1

		# reswap if needed, if score did not rise
		if score_change <= 0:
			self.slides[index1].photos[photo1] = photo1Object
			self.slides[index2].photos[photo2] = photo2Object
		else:
			self.score += score_change



		return [score_change, photo1Object.id, photo2Object.id]


	def score_from_slides(self, index1, index2):
		lower = min(index1, index2)
		upper = max(index1, index2)
		max_index = len(self.slides) - 1

		score_part = 0
		if lower > 0:
			score_part += Evaluator.get_transition_score(self.slides[lower-1], self.slides[lower])
		if lower < max_index:
			score_part += Evaluator.get_transition_score(self.slides[lower], self.slides[lower+1])

		if upper > 0:
			score_part += Evaluator.get_transition_score(self.slides[upper-1], self.slides[upper])
		if upper < max_index:
			score_part += Evaluator.get_transition_score(self.slides[upper], self.slides[upper+1])

		return score_part


def print_log(line, log_file='log.txt'):
	with open(log_file, 'a') as file_out:
		file_out.write(line + '\n')


def dict_increment(dict, key, increment = 1):
	if key in dict:
		dict[key] += increment
	else:
		dict[key] = increment


def read_photo(id):
	words = input().split()
	horizontal = False
	if words[0] == 'H':
		horizontal = True
	tags_count = int(words[1])
	tags_dict = {}
	for i in range(tags_count):
		tag = words[2 + i]
		tags_dict[tag] = 1
		dict_increment(all_tags_dict, tag)

	return Photo(id, horizontal, tags_dict)

def photos_with_tag(tag):
	# return filter(lambda photo: photo.has_tag(tag), photos)
	return filter(lambda photo: photo.has_tag(tag) and photo.horizontal, photos)

def ids_with_tag(tag):
	ids = []
	# photos_with_ids = filter(lambda photo: photo.has_tag(tag), photos)
	photos_with_ids = filter(lambda photo: photo.has_tag(tag) and photo.horizontal, photos)
	return map(lambda photo: photo.id, photos_with_ids)


photos_count = int(input())
photos = [None] * photos_count
all_tags_dict = {}

for i in range(photos_count):
	photos[i] = read_photo(i)

tags_by_occurence = sorted(all_tags_dict.items(), key=operator.itemgetter(1))
most_occurent_tag = tags_by_occurence[-1][0]
most_occurent_tag_photos = list(photos_with_tag(most_occurent_tag))

photos_horizontal = list(filter(lambda photo: photo.horizontal, photos))
photos_vertical = list(filter(lambda photo: photo.horizontal == False, photos))


slides_horizontal = list(map(lambda photo: Slide([photo]), photos_horizontal))
slides_vertical = [None] * (len(photos_vertical)//2)
for i in range(len(photos_vertical)//2):
	slides_vertical[i] = Slide([photos_vertical[2*i], photos_vertical[2*i+1]])

slides = slides_horizontal + slides_vertical

slideshow = [None] * len(slides)

slideshow[0] = slides[0]
slides[0].used = True

def max_transition_next_unused_slide(slide1, slides):
	max_transition_score = 0
	max_slide_index = 0

	for i, slide in enumerate(slides):
		if slide.used:
			continue
		slide_score = Evaluator.get_transition_score(slide1, slide)
		if slide_score >= max_transition_score:
			max_transition_score = slide_score
			max_slide_index = i
		
	return [max_slide_index, max_transition_score]

for i in range(len(slideshow)-1):
	[slide_index, slide_score] = max_transition_next_unused_slide(slideshow[i], slides)
	slideshow[i+1] = slides[slide_index]
	slides[slide_index].used = True
	print('Slideshow index: ' + str(i))
	print('Added slide index: ' + str(slide_index))
	print('Added slide score: ' + str(slide_score))

evaluator = Evaluator(slideshow)
print('# starting score: ' + str(evaluator.get_score()))
evaluator.incremental_run(10**6)

print('# score: ' + str(evaluator.get_score()))
evaluator.print_out()

print_log('')
print_log('script terminated')
print_log('-----------------------------------------------------------\n\n\n\n')
