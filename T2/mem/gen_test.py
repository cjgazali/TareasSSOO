import random

with open("test.txt", "w") as file:
	for i in range(150):
		file.write("{}\n".format(random.randrange(0, 268435455)))