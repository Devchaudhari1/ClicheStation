from pathlib import Path

WAV_ROOT = Path("wav")


def enum_value(name):
    """
    Convert a WAV filename into a valid C++ enum identifier.
    """
    name = name.replace("gretschDrumset_", "")
    name = name.replace("#", "S")
    name = name.replace("-", "_")

    # Replace spaces and other invalid characters
    result = ""
    for char in name:
        if char.isalnum() or char == "_":
            result += char
        else:
            result += "_"

    # Avoid identifiers starting with a number
    if result and result[0].isdigit():
        result = "Sample" + result

    return result


for folder in sorted(WAV_ROOT.iterdir()):
    if not folder.is_dir():
        continue

    enum_name = folder.name.capitalize() + "Samples"
    map_name = folder.name.capitalize() + "SampleFiles"

    wav_files = sorted(folder.glob("*.wav"))

    if not wav_files:
        continue

    print("#include <QHash>")
    print("#include <QString>")
    print()

    # Enum
    print(f"enum class {enum_name} {{")

    for wav_file in wav_files:
        value = enum_value(wav_file.stem)
        print(f"    {value},")

    print("};")
    print()

    # Hash map
    print(
        f"inline const QHash<{enum_name}, QString> {map_name} = {{"
    )

    for wav_file in wav_files:
        value = enum_value(wav_file.stem)
        original_name = wav_file.stem

        print(
            f'    {{ {enum_name}::{value}, "{original_name}" }},'
        )

    print("};")
    print()
