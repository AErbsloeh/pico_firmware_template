from os import mkdir
from shutil import copyfile
from os.path import join, isdir
from datetime import datetime

from generator.helper.translater import get_embedded_datatype, replace_variables_with_parameters


def generate_fir_allpass_c(data_bitsize: int, data_signed: bool, sampling_rate: float, t_dly: float,
                           filter_id='', file_name='filter_fir_all', path2save='', define_path='src') -> None:
    """Generating C files for IIR filtering on microcontroller
    Args:
        data_bitsize:   Used quantization level for data stream
        data_signed:    Decision if LUT values are signed [otherwise unsigned]
        sampling_rate:  Sampling clock of data stream processing
        t_dly:          Value of achievable time delay [in s]
        filter_id:      ID of used filter structure
        file_name:      Name of the generated files
        path2save:      Path for saving the verilog_filter output files
        define_path:    Path for loading the header file in IDE [Default: 'src']
    Return:
        None
    """
    module_id_used = filter_id if filter_id else '0'
    data_type_filter = get_embedded_datatype(data_bitsize, data_signed)
    template_c = __generate_filter_fir_allpass_template()
    filter_order = int(sampling_rate * t_dly)

    params = {
        'datetime_created':     datetime.now().strftime("%m/%d/%Y, %H:%M:%S"),
        'path2include':         define_path,
        'template_name':        'filter_fir_all_template.h',
        'device_id':            module_id_used.upper(),
        'data_type':            data_type_filter,
        'fs':                   f'{sampling_rate}',
        't_dly':                str(t_dly * 1e6),
        'filter_order':         str(filter_order),
    }
    proto_file = replace_variables_with_parameters(template_c['head'], params)
    imple_file = replace_variables_with_parameters(template_c['func'], params)

    # --- Write lines to file
    used_template_name = params["template_name"]
    copyfile(f'./../template_c/{used_template_name}', join(path2save, f'{used_template_name}'))
    # Checking if path is available
    if path2save and not isdir(path2save):
        mkdir(path2save)

    # --- Write lines to file
    with open(join(path2save, f'{file_name}{module_id_used.lower()}.h'), 'w') as v_handler:
        for line in proto_file:
            v_handler.write(line + '\n')
    v_handler.close()

    with open(join(path2save, f'{file_name}{module_id_used.lower()}.c'), 'w') as v_handler:
        for line in imple_file:
            v_handler.write(line + '\n')
    v_handler.close()


def __generate_filter_fir_allpass_template() -> dict:
    """Generate the template for writing *.c and *.h file for generate a FIR filter on MCUs
    Return:
        Dictionary with infos for prototype ['head'], implementation ['func'] and used parameters ['params']
    """
    header_temp = [
        f'// --- Generating a FIR-Allpass filter template',
        '// Copyright @ UDE-IES',
        '// Code generated on: {$datetime_created}',
        '// Params: N = {$filter_order}, t_dly = {$t_dly} us @ {$fs} Hz',
        '# include "{$path2include}/{$template_name}"',
        'DEF_NEW_FIR_ALL_FILTER_PROTO({$device_id}, {$data_type})'
    ]
    func_temp = [
        f'// --- Generating a FIR-Allpass filter template',
        '// Copyright @ UDE-IES',
        '// Code generated on: {$datetime_created}',
        '// Params: N = {$filter_order}, t_dly = {$t_dly} us @ {$fs} Hz',
        '# include "{$path2include}/{$template_name}"',
        'DEF_NEW_FIR_ALL_FILTER_IMPL({$device_id}, {$data_type}, {$filter_order})'
    ]

    return {'head': header_temp, 'func': func_temp, 'params': []}


if __name__ == '__main__':
    path2save = '../../build'
    generate_fir_allpass_c(16, True, 1e3, 10e-3, path2save=path2save)
