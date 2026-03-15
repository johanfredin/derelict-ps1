from PIL import Image
import numpy as np
import argparse


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Make background transparent and crop to foreground bounds."
    )
    parser.add_argument("input", help="Input image path")
    parser.add_argument("output", help="Output image path")
    parser.add_argument(
        "--threshold",
        type=float,
        default=20.0,
        help="Color distance threshold for separating foreground from background",
    )
    args = parser.parse_args()

    img = Image.open(args.input).convert("RGBA")
    arr = np.array(img)

    rgb = arr[:, :, :3].astype(np.float32)
    h, w = rgb.shape[:2]

    # Estimate background from border pixels instead of whole-image average.
    border_pixels = np.concatenate(
        [
            rgb[0, :, :],
            rgb[h - 1, :, :],
            rgb[:, 0, :],
            rgb[:, w - 1, :],
        ],
        axis=0,
    )
    bg = np.mean(border_pixels, axis=0)

    # Foreground mask based on color distance from background.
    dist = np.linalg.norm(rgb - bg, axis=2)
    mask = dist > args.threshold

    # If no foreground is detected, save the original image and exit.
    if not np.any(mask):
        img.save(args.output)
        print(args.output)
        return

    # Apply transparency: foreground stays opaque, background becomes transparent.
    result = arr.copy()
    result[:, :, 3] = np.where(mask, 255, 0).astype(np.uint8)

    # Crop to the bounds of the detected foreground.
    ys, xs = np.where(mask)
    y1, y2 = ys.min(), ys.max()
    x1, x2 = xs.min(), xs.max()

    cropped = result[y1:y2 + 1, x1:x2 + 1]

    out = Image.fromarray(cropped, "RGBA")
    out.save(args.output)

    print(args.output)


if __name__ == "__main__":
    main()
