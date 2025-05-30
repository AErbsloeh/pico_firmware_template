from os import mkdir
from shutil import copyfile
from os.path import join, isdir
from datetime import datetime

from generator.helper.emulator_filter import FilterStageGenerator
from generator.helper.translater import get_embedded_datatype, replace_variables_with_parameters


def generate_fir_filter_c(data_bitsize: int, data_signed: bool,
                          filter_order: int, sampling_rate: float, filter_corner: list,
                          filter_btype='low', filter_ftype='butter',
                          do_optimized=False, filter_id='', file_name='filter_fir',
                          path2save='', define_path='src') -> None:
    """Generating C files for IIR filtering on microcontroller
    Args:
        data_bitsize:   Used quantization level for data stream
        data_signed:    Decision if LUT values are signed [otherwise unsigned]
        filter_id:      ID of used filter structure
        filter_order:   Order of the filter
        sampling_rate:  Sampling clock of data stream processing
        filter_corner:  List with corner frequency of the used filter
        filter_btype:   Used filter type ['low', 'high', 'bandpass', 'bandstop']
        filter_ftype:   Used filter design ['butter', 'cheby1', 'cheby2', 'ellip', 'bessel']
        do_optimized:   Decision if LUT resources should be minimized [only quarter and mirroring]
        file_name:      Name of the generated files
        path2save:      Path for saving the verilog_filter output files
        define_path:    Path for loading the header file in IDE [Default: 'src']
    Return:
        None
    """
    module_id_used = filter_id if filter_id else '0'
    if do_optimized and filter_order % 2 == 0:
        raise NotImplementedError("Please add an odd number to filter order!")

    filter_emulator = FilterStageGenerator(filter_order, sampling_rate, filter_corner, False,
                                           ftype=filter_ftype, btype=filter_btype)
    filter_coeff = filter_emulator.get_coeff_full()
    filter_coeff_used = filter_coeff['coeffb'] if not do_optimized else filter_coeff['coeffb'][:int(filter_order/2)+1]

    data_type_filter = get_embedded_datatype(data_bitsize, data_signed)
    template_c = __generate_filter_fir_template(do_optimized)

    params = {
        'datetime_created':     datetime.now().strftime("%m/%d/%Y, %H:%M:%S"),
        'path2include':         define_path,
        'template_name':        'filter_fir_template.h',
        'device_id':            module_id_used.upper(),
        'data_type':            data_type_filter,
        'fs':                   f'{sampling_rate}',
        'filter_type':          f'{filter_btype}, {filter_ftype}',
        'filter_corner':        ', '.join(map(str, filter_corner)),
        'filter_order':         str(filter_order),
        'coeffs_string':        ', '.join(map(str, filter_coeff_used))
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


def __generate_filter_fir_template(do_opt: bool) -> dict:
    """Generate the template for writing *.c and *.h file for generate a FIR filter on MCUs
    Args:
        do_opt:     Boolean decision if optimized version is used (odd version)
    Return:
        Dictionary with infos for prototype ['head'], implementation ['func'] and used parameters ['params']
    """
    version_fir = 'full' if not do_opt else 'opt'
    header_temp = [
        f'// --- Generating a FIR filter template ({version_fir})',
        '// Copyright @ UDE-IES',
        '// Code generated on: {$datetime_created}',
        '// Params: N = {$filter_order}, f_c = [{$filter_corner}] Hz @ {$fs} Hz ({$filter_type})',
        '// Used filter coefficient order (b_0, b_1, b_2, ..., b_N)',
        '# include "{$path2include}/{$template_name}"',
        'DEF_NEW_FIR_FILTER_PROTO({$device_id}, {$data_type})'
    ]
    func_temp = [
        f'// --- Generating a FIR filter template ({version_fir})',
        '// Copyright @ UDE-IES',
        '// Code generated on: {$datetime_created}',
        '// Params: N = {$filter_order}, f_c = [{$filter_corner}] Hz @ {$fs} Hz ({$filter_type})',
        '// Used filter coefficient order (b_0, b_1, b_2, ..., b_N)',
        '# include "{$path2include}/{$template_name}"',
        'DEF_NEW_FIR_FILTER_IMPL({$device_id}, {$data_type}, {$filter_order}, {$coeffs_string})'
    ]
    return {'head': header_temp, 'func': func_temp, 'params': []}


if __name__ == '__main__':
    path2save = '../../build'
    generate_fir_filter_c(14, True, 21, 1e3, [100], do_optimized=True, path2save=path2save)
