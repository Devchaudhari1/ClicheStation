from PIL import Image
import os

input_file = input("Enter image path: ")

# -----------------------------
# Settings
# -----------------------------
columns = 7
rows = 5

first_row_height = 182
scale = 4
threshold = 100

# -----------------------------
# Open image
# -----------------------------
img = Image.open(input_file).convert("RGB")

width, height = img.size

# Check that first row fits
if first_row_height >= height:
    raise ValueError("First row height is larger than the image height.")

# Remaining height divided among rows 2-5
remaining_height = height - first_row_height
other_row_height = remaining_height // (rows - 1)

print(f"Image size: {width} x {height}")
print(f"First row: {first_row_height}px")
print(f"Other rows: approximately {other_row_height}px")

# Output folder
output_dir = "processed_tiles"
os.makedirs(output_dir, exist_ok=True)

# Equal column widths
column_width = width // columns

# -----------------------------
# Process tiles
# -----------------------------
for row in range(rows):

    # Vertical coordinates
    if row == 0:
        top = 0
        bottom = first_row_height
    else:
        top = first_row_height + (row - 1) * other_row_height

        # Make final row include any leftover pixels
        if row == rows - 1:
            bottom = height
        else:
            bottom = first_row_height + row * other_row_height

    for col in range(columns):

        # Horizontal coordinates
        left = col * column_width

        # Make final column include any leftover pixels
        if col == columns - 1:
            right = width
        else:
            right = (col + 1) * column_width

        # Crop
        tile = img.crop((left, top, right, bottom))

        # -----------------------------
        # Upscale
        # -----------------------------
        new_size = (
            tile.width * scale,
            tile.height * scale
        )

        upscaled = tile.resize(
            new_size,
            Image.Resampling.LANCZOS
        )

        # -----------------------------
        # Grayscale
        # -----------------------------
        gray = upscaled.convert("L")

        # -----------------------------
        # Threshold
        # -----------------------------
        alpha = gray.point(
            lambda p: 255 if p < threshold else 0
        )

        # -----------------------------
        # Transparent PNG
        # -----------------------------
        result = Image.new(
            "RGBA",
            new_size,
            (0, 0, 0, 0)
        )

        result.putalpha(alpha)

        # -----------------------------
        # Save
        # -----------------------------
        filename = f"tile_{row + 1}_{col + 1}.png"
        output_path = os.path.join(output_dir, filename)

        result.save(output_path)

        print(
            f"Row {row + 1}, Col {col + 1}: "
            f"{tile.width} x {tile.height} -> "
            f"{new_size[0]} x {new_size[1]}"
        )

print("\nDone!")
print(f"Processed {rows * columns} tiles.")