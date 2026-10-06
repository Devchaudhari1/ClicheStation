from PIL import Image

input_file = input("Enter image path: ")

# Open as RGB
img = Image.open(input_file).convert("RGB")

# Upscale first
scale = 4
new_size = (img.width * scale, img.height * scale)

upscaled = img.resize(
    new_size,
    Image.Resampling.LANCZOS
)

# Convert the UPSCALED image to grayscale
gray = upscaled.convert("L")

# Threshold
threshold = 100

# Black pixels -> opaque
# Grey/white pixels -> transparent
alpha = gray.point(lambda p: 255 if p < threshold else 0)

# Create transparent RGBA image
result = Image.new("RGBA", new_size, (0, 0, 0, 0))

# Apply transparency mask
result.putalpha(alpha)

# Save
result.save(input_file)

print(f"Saved: {input_file}")