import os
import argparse
from PIL import Image, ImageDraw, ImageFont

def run_renders(args, color_values, firing_values):
    original_scene_dir = os.path.dirname(args.scene_file)

    # Lê o arquivo base
    with open(args.scene_file, "r") as f:
        base_scene = f.read()

    # Cria o diretório de saída, se não existir
    os.makedirs(args.out_dir, exist_ok=True)

    for color in color_values:
        for firing in firing_values:
            modified_scene = base_scene.replace("__COLOR__", str(color)).replace("__FIRING__", str(firing))

            # Nome do arquivo
            modified_scene_path = os.path.join(original_scene_dir, "modified_scene.pbrt")

            # Salva cena modificada
            with open(modified_scene_path, "w") as f:
                f.write(modified_scene)

            # Define nome do arquivo de imagem de saída
            output_filename = f"color{color:.1f}_firing{firing:.2f}.png"
            output_path = os.path.join(args.out_dir, output_filename)

            # Comando PBRT com caminho de saída
            cmd = f"{args.pbrt_bin} --log-level fatal {"--gpu" if args.gpu else ""} -spp {args.spp} --outfile {output_path} {modified_scene_path}"
            print(f"Rendering: color={color}, firing={firing}")
            os.system(cmd)


def generate_matrix(args, color_values, firing_values):
    print("Generating matrix...")
    sample_path = os.path.join(args.out_dir, f"color{color_values[0]:.1f}_firing{firing_values[0]:.2f}.png")
    if not os.path.exists(sample_path):
        raise FileNotFoundError(f"Sample image not found: {sample_path}")
    sample_img = Image.open(sample_path).convert("RGB")
    img_w, img_h = sample_img.size

    # Margens para texto
    label_margin_x = 280    # espaço para colunas de rótulo à esquerda
    label_margin_y = 100    # espaço para linha de rótulo no topo

    # Tamanho da imagem total com espaço para rótulos
    grid_w = img_w * len(firing_values) + label_margin_x
    grid_h = img_h * len(color_values) + label_margin_y
    grid_img = Image.new("RGB", (grid_w, grid_h), color=(255, 255, 255))

    draw = ImageDraw.Draw(grid_img)

    # Desenha as imagens e os rótulos
    for row, color in enumerate(color_values):
        for col, firing in enumerate(firing_values):
            filename = f"color{color:.1f}_firing{firing:.2f}.png"
            path = os.path.join(args.out_dir, filename)
            if os.path.exists(path):
                img = Image.open(path).convert("RGB")
                x = col * img_w + label_margin_x
                y = row * img_h + label_margin_y
                grid_img.paste(img, (x, y))
            else:
                print(f"[Warning] Image not found: {filename}")

    font = ImageFont.load_default(52)

    # Escreve os rótulos das linhas (color)
    for row, color in enumerate(color_values):
        y = row * img_h + label_margin_y + img_h // 2 - 10
        draw.text((10, y), f"color {color:.1f}", fill="black", font=font)

    # Escreve os rótulos das colunas (firing)
    for col, firing in enumerate(firing_values):
        x = col * img_w + label_margin_x + img_w // 2 - 30
        draw.text((x, 10), f"firing {firing:.2f}", fill="black", font=font)

    # Salva a imagem final
    grid_img.save(os.path.join(args.out_dir, "matrix.png"))
    print(f"[OK] Matrix with labels saved to: {args.out_dir}")


def main():
    # Define e interpreta os argumentos de linha de comando
    parser = argparse.ArgumentParser(description="Render PBRT scenes with varying color and firing parameters.")
    parser.add_argument("--color_delta", type=float, required=False, default=0.5, help="Color delta")
    parser.add_argument("--firing_delta", type=float, required=False, default=0.25, help="Firing delta")
    parser.add_argument("--initial_color", type=float, required=False, default=0, help="Initial color")
    parser.add_argument("--initial_firing", type=float, required=False, default=0, help="Initial firing")
    parser.add_argument("--pbrt_bin", type=str, required=True, help="Path to the PBRT binary")
    parser.add_argument("--scene_file", type=str, required=True, help="Path to the base PBRT scene file")
    parser.add_argument("--out_dir", type=str, required=True, help="Directory where rendered scenes and outputs will be stored")
    parser.add_argument("--spp", type=str, required=False, default=64, help="Samples per pixel")
    parser.add_argument("--gpu", action="store_true", default=False, help="Use GPU acceleration")
    parser.add_argument(
        "--generate_matrix",
        action="store_true",
        default=False,
        help="Generate a matrix image that combines all rendered outputs into a single grid."
    )

    args = parser.parse_args()

    pbrt_path = os.path.abspath(args.pbrt_bin)
    if not os.path.exists(pbrt_path):
        raise FileNotFoundError(f"Binary file {pbrt_path} does not exist")

    original_scene_dir = os.path.abspath(args.scene_file)
    if not os.path.exists(original_scene_dir):
        raise FileNotFoundError(f"Scene file {original_scene_dir} does not exist")

    color = args.initial_color
    color_values = []
    while color < 7:
        color_values.append(color)
        color += args.color_delta

    firing = args.initial_firing
    firing_values = []
    while firing <= 1:
        firing_values.append(firing)
        firing += args.firing_delta

    run_renders(args, color_values, firing_values)
    if args.generate_matrix:
        generate_matrix(args, color_values, firing_values)


if __name__ == "__main__":
    main()
