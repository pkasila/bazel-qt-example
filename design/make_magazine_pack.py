#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageEnhance, ImageFilter, ImageOps


SUPPORTED_EXTENSIONS = {".jpg", ".jpeg", ".png", ".webp", ".tif", ".tiff"}


def discover_images(input_dir: Path) -> list[Path]:
    return sorted(
        path
        for path in input_dir.iterdir()
        if path.is_file() and path.suffix.lower() in SUPPORTED_EXTENSIONS
    )


def ensure_rgb(image: Image.Image) -> Image.Image:
    if image.mode == "RGB":
        return image
    if image.mode in {"RGBA", "LA"}:
        background = Image.new("RGB", image.size, (255, 255, 255))
        background.paste(image, mask=image.getchannel("A"))
        return background
    return image.convert("RGB")


def magazine_grade(image: Image.Image) -> Image.Image:
    image = ensure_rgb(image)

    # Gentle global treatment only: no retouching, no object changes.
    image = ImageOps.exif_transpose(image)
    image = ImageOps.autocontrast(image, cutoff=0.4)
    image = ImageEnhance.Color(image).enhance(1.03)
    image = ImageEnhance.Contrast(image).enhance(1.05)
    image = ImageEnhance.Brightness(image).enhance(1.02)
    image = ImageEnhance.Sharpness(image).enhance(1.06)

    # A tiny amount of clarity helps interiors read better in print.
    return image.filter(ImageFilter.UnsharpMask(radius=1.3, percent=50, threshold=3))


def fit_inside(size: tuple[int, int], frame: tuple[int, int]) -> tuple[int, int]:
    width, height = size
    max_width, max_height = frame
    scale = min(max_width / width, max_height / height)
    return max(1, int(width * scale)), max(1, int(height * scale))


def make_pdf_page(image: Image.Image, title: str) -> Image.Image:
    portrait = image.height >= image.width
    page_size = (2480, 3508) if portrait else (3508, 2480)  # A4 at 300 DPI
    margin = 180
    shadow_offset = 18

    page = Image.new("RGB", page_size, "#f5f1ea")
    frame_size = (page_size[0] - margin * 2, page_size[1] - margin * 2)
    fitted_size = fit_inside(image.size, frame_size)
    fitted = image.resize(fitted_size, Image.Resampling.LANCZOS)

    x = (page_size[0] - fitted.width) // 2
    y = (page_size[1] - fitted.height) // 2

    shadow = Image.new("RGBA", (fitted.width + shadow_offset * 2, fitted.height + shadow_offset * 2), (0, 0, 0, 0))
    shadow_box = Image.new("RGBA", (fitted.width, fitted.height), (0, 0, 0, 70))
    shadow.paste(shadow_box, (shadow_offset, shadow_offset))
    shadow = shadow.filter(ImageFilter.GaussianBlur(20))

    page_rgba = page.convert("RGBA")
    page_rgba.alpha_composite(shadow, (x - shadow_offset, y - shadow_offset))

    white_border = ImageOps.expand(fitted, border=18, fill="white")
    page_rgb = page_rgba.convert("RGB")
    page_rgb.paste(white_border, (x - 18, y - 18))
    return page_rgb


def process_images(input_dir: Path, output_dir: Path, pdf_name: str) -> tuple[list[Path], Path]:
    images = discover_images(input_dir)
    if not images:
        raise FileNotFoundError(
            f"No images found in {input_dir}. Put JPG/PNG/WebP/TIFF files there and run again."
        )

    processed_dir = output_dir / "processed"
    processed_dir.mkdir(parents=True, exist_ok=True)

    processed_paths: list[Path] = []
    pdf_pages: list[Image.Image] = []

    for source_path in images:
        with Image.open(source_path) as source_image:
            graded = magazine_grade(source_image)

            output_path = processed_dir / f"{source_path.stem}_magazine.jpg"
            graded.save(output_path, "JPEG", quality=95, subsampling=0, optimize=True)
            processed_paths.append(output_path)

            pdf_pages.append(make_pdf_page(graded, source_path.stem))

    pdf_path = output_dir / pdf_name
    pdf_pages[0].save(
        pdf_path,
        "PDF",
        save_all=True,
        append_images=pdf_pages[1:],
        resolution=300.0,
    )
    return processed_paths, pdf_path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Applies gentle magazine-style grading to interior photos and packs them into a PDF."
    )
    parser.add_argument(
        "--input-dir",
        default="input",
        type=Path,
        help="Folder with source images.",
    )
    parser.add_argument(
        "--output-dir",
        default="output",
        type=Path,
        help="Folder for processed images and the final PDF.",
    )
    parser.add_argument(
        "--pdf-name",
        default="interior_magazine_portfolio.pdf",
        help="Name of the final PDF file.",
    )
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    try:
        processed_paths, pdf_path = process_images(args.input_dir, args.output_dir, args.pdf_name)
    except FileNotFoundError as exc:
        parser.error(str(exc))
        return 2

    print(f"Processed {len(processed_paths)} image(s).")
    print(f"Images: {args.output_dir / 'processed'}")
    print(f"PDF: {pdf_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
