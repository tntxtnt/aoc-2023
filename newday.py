import shutil, sys, os

def replace_str_in_file(filename, old_str, new_str):
    os.rename(filename, filename + '.old')
    with open(filename + '.old', 'rt') as fin:
        with open(filename, 'wt') as fout:
            for line in fin:
                fout.write(line.replace(old_str, new_str))
    os.remove(filename + '.old')

def add_day():
    cmakelists = 'CMakeLists.txt'
    os.rename(cmakelists, cmakelists + '.old')
    day_num = 0
    with open(cmakelists + '.old', 'rt') as fin:
        with open(cmakelists, 'wt') as fout:
            for line in fin:
                if line.startswith('add_subdirectory("day'):
                    day_num = int(line[21:-3])
                if line.strip() == '' and day_num > 0:
                    day_num += 1
                    fout.write(f'add_subdirectory("day{day_num}")')
                    fout.write('\n')
                fout.write(line)
    os.remove(cmakelists + '.old')
    return day_num

if __name__ == '__main__':
    day_num = add_day()
    dst = f'day{day_num}'
    shutil.copytree('template/dayn', dst)
    os.rename(dst + '/dayn.txt', dst + f'/day{day_num}.txt')
    os.rename(dst + '/src/dayn.cpp', dst + f'/src/day{day_num}.cpp')
    replace_str_in_file(dst + '/CMakeLists.txt', 'dayn', f'day{day_num}')
    replace_str_in_file(dst + f'/src/day{day_num}.cpp', 'dayn', f'day{day_num}')

