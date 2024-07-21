import os

def convert_lua_to_header(input_dir, output_dir, impl_file):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    with open(impl_file, 'w') as impl:
        impl.write('#include "lua_headers.h"\n\n')
        for file_name in os.listdir(input_dir):
            if file_name.endswith('.lua'):
                variable_name = file_name.replace('.lua', '_script').replace('.', '_')
                header_file_name = file_name.replace('.lua', '.h')
                with open(os.path.join(output_dir, header_file_name), 'w') as out_file:
                    out_file.write('#pragma once\n\n')
                    out_file.write(f'extern const char* {variable_name};\n')
                impl.write(f'const char* {variable_name} = R"(\n')
                with open(os.path.join(input_dir, file_name), 'r') as in_file:
                    for line in in_file:
                        impl.write(line)
                impl.write('\n)";\n\n')
                print(f'Generated header file: {header_file_name}')

scripts_dir = 'scripts'
headers_dir = 'lua_headers'
impl_file = 'lua_headers.cpp'
convert_lua_to_header(scripts_dir, headers_dir, impl_file)
print(f'Converted Lua scripts in {scripts_dir} to headers in {headers_dir} and implementation in {impl_file}')
