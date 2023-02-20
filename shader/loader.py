def loadFile(file: str):
    with open(file, 'rb') as f:
        return f.read()


if __name__ == '__main__':
    vert = f'{loadFile("./tracer.vert")}'
    frag = f'{loadFile("./tracer.frag")}'
    render = f'{loadFile("./render.frag")}'

    with open('./shaderBuf.h', 'w') as f:
        f.write(f'#pragma once\n'
                f'\n'
                f'#define tracer_vert "{vert[2:-1]}"\n'
                f'\n'
                f'#define tracer_frag "{frag[2:-1]}"\n'
                f'\n'
                f'#define render_frag "{render[2:-1]}"\n'
                f'')
