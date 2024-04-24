from quickdraw import QuickDrawData
from quickdraw import QuickDrawDataGroup
from pathlib import Path

image_size = (28, 28)
chosen_categories = ["tractor", "bicycle", "umbrella", "popsicle", "octopus", "cat", "house plant", "cruise ship", "helicopter", "windmill"]

def generate_class_images(name, max_drawings, recognized):
    directory = Path("training_data/" + name)

    if not directory.exists():
        directory.mkdir(parents=True)

    images = QuickDrawDataGroup(name, max_drawings=max_drawings, recognized=recognized)
    for img in images.drawings:
        filename = directory.as_posix() + "/" + str(img.key_id) + ".png"
        img.get_image(stroke_width=3).resize(image_size).save(filename)

for label in QuickDrawData().drawing_names:
    if (label in chosen_categories):
        generate_class_images(label, max_drawings=1200, recognized=True)
