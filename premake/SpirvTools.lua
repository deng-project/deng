--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: SpirvTools.lua - spirv-tools dependencies build config
--- author: Karl-Mihkel Ott

------------------------
--- Global constants ---
------------------------

-- include directories
local SPIRV_TOOLS_SOURCE_DIR            = "../deps/shaderc/third_party/spirv-tools"
local GENERATED_INCLUDE_DIR             = "%{cfg.buildtarget.directory}/GeneratedInclude"
local SPIRV_HEADER_INCLUDE_DIR          = "../deps/shaderc/third_party/spirv-headers/include"

-- script definitions
local GRAMMAR_PROCESSING_SCRIPT         = "../deps/shaderc/third_party/spirv-tools/utils/generate_grammar_tables.py"
local VIMSYNTAX_PROCESSING_SCRIPT       = "../deps/shaderc/third_party/spirv-tools/utils/generate_vim_syntax.py"
local XML_REGISTRY_PROCESSING_SCRIPT    = "../deps/shaderc/third_party/spirv-tools/utils/generate_registry_tables.py"
local LANG_HEADER_PROCESSING_SCRIPT     = "../deps/shaderc/third_party/spirv-tools/utils/generate_language_headers.py"

-- grammar file definitions
local DEBUGINFO_GRAMMAR_JSON_FILE       = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/unified1/extinst.debuginfo.grammar.json"
local CLDEBUGINFO100_GRAMMAR_JSON_FILE  = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/unified1/extinst.opencl.debuginfo.100.grammar.json"
local VKDEBUGINFO100_GRAMMAR_JSON_FILE  = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/unified1/extinst.nonsemantic.shader.debuginfo.100.grammar.json"

-- Generate core table include files
function GenerateCoreTables(config_version)
    local grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/spirv.core.grammar.json"
    local grammar_insts_inc_file = GENERATED_INCLUDE_DIR .. "/core.insts-" .. config_version .. ".inc"
    local grammar_kinds_inc_file = GENERATED_INCLUDE_DIR .. "/operand.kinds-" .. config_version .. ".inc"

    local cmd = "python3 " .. GRAMMAR_PROCESSING_SCRIPT ..
        " --spirv-core-grammar=" .. grammar_json ..
        " --extinst-debuginfo-grammar=" .. DEBUGINFO_GRAMMAR_JSON_FILE ..
        " --extinst-cldebuginfo100-grammar=" .. CLDEBUGINFO100_GRAMMAR_JSON_FILE ..
        " --core-insts-output=" .. grammar_insts_inc_file ..
        " --operand-kinds-output=" .. grammar_kinds_inc_file

    return cmd
end

-- Generate grammar enumerals --
function GenerateEnumStringMappings(config_version)
    local grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/spirv.core.grammar.json"
    local enum_inc_file = GENERATED_INCLUDE_DIR .. "/extension_enum.inc"
    local string_map_file = GENERATED_INCLUDE_DIR .. "/enum_string_mapping.inc"

    local cmd = "python3 " .. GRAMMAR_PROCESSING_SCRIPT ..
        " --spirv-core-grammar=" .. grammar_json ..
        " --extinst-debuginfo-grammar=" .. DEBUGINFO_GRAMMAR_JSON_FILE ..
        " --extinst-cldebuginfo100-grammar=" .. CLDEBUGINFO100_GRAMMAR_JSON_FILE ..
        " --extension-enum-output=" .. enum_inc_file ..
        " --enum-string-mapping-output=" .. string_map_file

    return cmd
end


function GenerateVendorTables(vendor_table, short_name, operand_kind_prefix)
    local insts_file = GENERATED_INCLUDE_DIR .. "/" .. vendor_table .. ".insts.inc"
    local grammar_file = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/unified1/extinst." .. vendor_table .. ".grammar.json"

    local cmd = "python3 " .. GRAMMAR_PROCESSING_SCRIPT ..
        " --extinst-vendor-grammar=" .. grammar_file ..
        " --vendor-insts-output=" .. insts_file ..
        " --vendor-operand-kind-prefix=" .. operand_kind_prefix

    return cmd
end


function GenerateLanguageHeaders(name, grammar_file)
    local out_h = GENERATED_INCLUDE_DIR .. "/" .. name .. ".h"

    local cmd = "python3 " .. LANG_HEADER_PROCESSING_SCRIPT ..
        " --extinst-grammar=" .. grammar_file ..
        " --extinst-output-path=" .. out_h

    return cmd
end


-- Generate vim syntax tree --
function GenerateVimSyntax(config_version, cl_version)
    local grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/spirv.core.grammar.json"
    local glsl_grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/extinst.glsl.std.450.grammar.json"
    local opencl_grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/extinst.opencl.std.100.grammar.json"
    local vimsyntax_file = GENERATED_INCLUDE_DIR .. "/spvasm.vim"

    local cmd = "python3 " .. VIMSYNTAX_PROCESSING_SCRIPT ..
        " --spirv-core-grammar=" .. grammar_json ..
        " --extinst-debuginfo-grammar=" .. DEBUGINFO_GRAMMAR_JSON_FILE ..
        " --extinst-glsl-grammar=" .. glsl_grammar_json ..
        " --extinst-opencl-grammar=" .. opencl_grammar_json ..
        " > " .. vimsyntax_file

    return cmd
end


function ExtractGeneratorList()
    local generator_inc = GENERATED_INCLUDE_DIR .. "/generators.inc"
    local spirv_xml_reg_file = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/spir-v.xml"

    local cmd = "python3 " .. XML_REGISTRY_PROCESSING_SCRIPT ..
        " --xml=" .. spirv_xml_reg_file ..
        " --generator-output=" .. generator_inc

    return cmd
end


function GenerateBuildVersion()
    local build_version_inc = GENERATED_INCLUDE_DIR .. "/build-version.inc"
    local build_version_inc_generator = "../deps/shaderc/third_party/spirv-tools/utils/update_build_version.py"
    local changes_files = GENERATED_INCLUDE_DIR .. "/CHANGES"

    local cmd = "python3 " .. build_version_inc_generator .. " " .. SPIRV_TOOLS_SOURCE_DIR .. " " .. build_version_inc
    return cmd
end


function GenerateGLSLTables(config_version)
    local glsl_grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/extinst.glsl.std.450.grammar.json"
    local grammar_inc_file = GENERATED_INCLUDE_DIR .. "/glsl.std.450.insts.inc"

    local cmd = "python3 " .. GRAMMAR_PROCESSING_SCRIPT ..
        " --extinst-glsl-grammar=" .. glsl_grammar_json ..
        " --glsl-insts-output=" .. grammar_inc_file

    return cmd
end


function GenerateOpenCLTables(config_version)
    local opencl_grammar_json = SPIRV_HEADER_INCLUDE_DIR .. "/spirv/" .. config_version .. "/extinst.opencl.std.100.grammar.json"
    local grammar_inc_file = GENERATED_INCLUDE_DIR .. "/opencl.std.insts.inc"

    local cmd = "python3 " .. GRAMMAR_PROCESSING_SCRIPT ..
        " --extinst-opencl-grammar=" .. opencl_grammar_json ..
        " --opencl-insts-output=" .. grammar_inc_file

    return cmd
end


project "SPIRV-Tools"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    -- Commands needed to generate includefiles for SPIRV-Tools-opt library --
    prebuildcommands {
        "{MKDIR} " .. GENERATED_INCLUDE_DIR,
        GenerateCoreTables("unified1"),
        GenerateEnumStringMappings("unified1"),
        GenerateOpenCLTables("unified1"),
        GenerateGLSLTables("unified1"),
        GenerateVendorTables("spv-amd-shader-explicit-vertex-parameter", "spv-amd-sevp", ""),
        GenerateVendorTables("spv-amd-shader-trinary-minmax", "spv-amd-stm", ""),
        GenerateVendorTables("spv-amd-gcn-shader", "spv-amd-gs", ""),
        GenerateVendorTables("spv-amd-shader-ballot", "spv-amd-sb", ""),
        GenerateVendorTables("debuginfo", "debuginfo", ""),
        GenerateVendorTables("opencl.debuginfo.100", "cldi100", "CLDEBUG100_"),
        GenerateVendorTables("nonsemantic.shader.debuginfo.100", "shdi100", "SHDEBUG100_"),
        GenerateVendorTables("nonsemantic.clspvreflection", "clspvreflection", ""),
        GenerateLanguageHeaders("DebugInfo", DEBUGINFO_GRAMMAR_JSON_FILE),
        GenerateLanguageHeaders("OpenCLDebugInfo100", CLDEBUGINFO100_GRAMMAR_JSON_FILE),
        GenerateLanguageHeaders("NonSemanticShaderDebugInfo100", VKDEBUGINFO100_GRAMMAR_JSON_FILE),
        GenerateVimSyntax("unified1", "1.0"),
        ExtractGeneratorList(),
        GenerateBuildVersion()
    }

    includedirs { 
        "../deps/shaderc/third_party/spirv-tools",
        "../deps/shaderc/third_party/spirv-tools/include",
        "../deps/shaderc/third_party/spirv-headers/include",
        GENERATED_INCLUDE_DIR
    }

    files {
        "../deps/shaderc/third_party/spirv-tools/source/util/bit_vector.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/util/parse_number.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/util/string_utils.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/assembly_grammar.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/binary.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/diagnostic.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/disassemble.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/enum_string_mapping.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/ext_inst.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/extensions.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/libspirv.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/name_mapper.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opcode.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/operand.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/parsed_operand.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/print.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/software_version.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/spirv_endian.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/spirv_fuzzer_options.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/spirv_optimizer_options.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/spirv_reducer_options.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/spirv_target_env.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/spirv_validator_options.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/table.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/text.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/text_handler.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_adjacency.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_annotation.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_arithmetics.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_atomics.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_barriers.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_bitwise.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_builtins.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_capability.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_cfg.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_composites.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_constants.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_conversion.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_debug.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_decorations.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_derivatives.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_extensions.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_execution_limitations.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_function.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_id.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_image.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_interfaces.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_instruction.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_layout.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_literals.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_logicals.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_memory.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_memory_semantics.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_misc.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_mode_setting.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_non_uniform.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_primitives.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_scopes.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_small_type_uses.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validate_type.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/decoration.h",
        "../deps/shaderc/third_party/spirv-tools/source/val/basic_block.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/construct.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/function.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/instruction.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/val/validation_state.cpp"
    }

-- Add SPIRV-Tools-opt library build config --
project "SPIRV-Tools-opt"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"


    includedirs { 
        "../deps/shaderc/third_party/spirv-tools",
        "../deps/shaderc/third_party/spirv-tools/include",
        "../deps/shaderc/third_party/spirv-headers/include",
        GENERATED_INCLUDE_DIR
    }

    files {
        "../deps/shaderc/third_party/spirv-tools/source/opt/aggressive_dead_code_elim_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/amd_ext_to_khr.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/basic_block.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/block_merge_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/block_merge_util.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/build_module.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/ccp_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/cfg_cleanup_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/cfg.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/code_sink.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/combine_access_chains.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/compact_ids_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/composite.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/const_folding_rules.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/constants.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/control_dependence.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/convert_to_sampled_image_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/convert_to_half_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/copy_prop_arrays.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/dataflow.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/dead_branch_elim_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/dead_insert_elim_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/dead_variable_elimination.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/decoration_manager.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/debug_info_manager.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/def_use_manager.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/desc_sroa.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/desc_sroa_util.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/dominator_analysis.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/dominator_tree.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_constant_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_functions_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_functions_util.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/eliminate_dead_members_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/feature_manager.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/fix_storage_class.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/flatten_decoration_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/fold.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/folding_rules.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/fold_spec_constant_op_and_composite_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/freeze_spec_constant_value_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/function.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/graphics_robust_access_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/if_conversion.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/inline_exhaustive_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/inline_opaque_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/inline_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/inst_bindless_check_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/inst_buff_addr_check_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/inst_debug_printf_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/instruction.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/instruction_list.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/instrument_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/interp_fixup_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/ir_context.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/ir_loader.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/licm_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/local_access_chain_convert_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/local_redundancy_elimination.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/local_single_block_elim_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/local_single_store_elim_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_dependence.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_dependence_helpers.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_descriptor.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_fission.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_fusion.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_fusion_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_peeling.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_utils.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_unroller.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/loop_unswitch_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/mem_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/merge_return_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/module.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/optimizer.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/pass_manager.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/private_to_local_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/propagator.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/reduce_load_size.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/redundancy_elimination.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/register_pressure.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/relax_float_ops_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/remove_duplicates_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/remove_unused_interface_variables_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/replace_desc_array_access_using_var_index.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/replace_invalid_opc.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/scalar_analysis.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/scalar_analysis_simplification.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/scalar_replacement_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/set_spec_constant_default_value_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/simplification_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/ssa_rewrite_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/strength_reduction_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/strip_debug_info_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/strip_nonsemantic_info_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/struct_cfg_analysis.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/type_manager.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/types.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/unify_const_pass.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/upgrade_memory_model.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/value_number_table.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/vector_dce.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/workaround1209.cpp",
        "../deps/shaderc/third_party/spirv-tools/source/opt/wrap_opkill.cpp"
    }

    links { "SPIRV-Tools" }
