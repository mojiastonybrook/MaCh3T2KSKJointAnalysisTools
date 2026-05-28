import os
import sys

def main():
    print("\n")

    work_dir = os.environ['PWD']
    #check if template files exist
    if ((not os.path.exists(work_dir+"/AtmConfig_SystLLHScan_SKOnly_OscOnly_HPD_TEMPLATE.cfg")) or (not os.path.exists(work_dir+"/Oscillator_TEMPLATE.cfg"))):
        print("No existing templates...")
        quit()

    oscillator_dir = "/home/mojia/projects/rpp-blairt2k/mojia/OscillatorBinningChange"
    #find the oscillator template
    try:
        oscillator_temp_coarse = input("coarse ??x??[61x52] :")
    except:
        oscillator_temp_coarse = "61x52"
        print("using default setup 61x52 for coarse oscillogram.\n")
    try:
        oscillator_temp_fine = input("coarse ??x??[244x208] :")
    except:
        oscillator_temp_fine = "244x208"
        print("using default setup 244x208 for fine oscillogram.\n")

    if (os.path.exists(oscillator_dir+"/OscillogramTemplate_coarse_"+oscillator_temp_coarse+"_fine_"+oscillator_temp_fine+".root") == True):
        print("Found existing templates: "+"OscillogramTemplate_coarse_"+oscillator_temp_coarse+"_fine_"+oscillator_temp_fine+".root")
    else:
        print("Invalid oscillogram template!")
        quit()
    #production height averaging 
    PHA_opt="on"    # "off"
    if PHA_opt == "on":
        usePHA = "true"
    else:
        usePHA = "false"

    #make directory for the templates
    temp_dir = work_dir+"/coarse"+oscillator_temp_coarse+"_fine"+oscillator_temp_fine+"_PHA"+PHA_opt
    command_mkdir = "mkdir -p "+temp_dir
    os.system(command_mkdir)
    config_dir = temp_dir+"/configs"
    command_mkdir = "mkdir -p "+config_dir
    os.system(command_mkdir)
    #copy the templates to the directory and make changes to the contents
    ##oscillator configuration card
    oscillator_config_temp = work_dir+"/Oscillator_TEMPLATE.cfg"
    oscillator_config = config_dir+"/Oscillator_coarse_"+oscillator_temp_coarse+"_fine_"+oscillator_temp_fine+".cfg"
    command_cp = "cp "+oscillator_config_temp+" "+oscillator_config
    os.system(command_cp)
    ###change contents
    oscillator_input_file=oscillator_dir+"/OscillogramTemplate_coarse_"+oscillator_temp_coarse+"_fine_"+oscillator_temp_fine+".root"
    command_sed = "sed -i 's|OscillatorInputFileName.*|OscillatorInputFileName = \""+oscillator_input_file+"\"|' "+oscillator_config
    os.system(command_sed)
    command_sed = "sed -i 's|OscillatorUseProductionHeightAveraging.*|OscillatorUseProductionHeightAveraging = "+usePHA+"|' "+oscillator_config
    os.system(command_sed)
    
    ##exec configuration card
    atm_config_temp = work_dir+"/AtmConfig_SystLLHScan_SKOnly_OscOnly_HPD_TEMPLATE.cfg"
    atm_config = config_dir+"/AtmConfig_SystLLHScan_SKOnly_OscOnly_HPD_coarse"+oscillator_temp_coarse+"_fine"+oscillator_temp_fine+".cfg"
    command_cp = "cp "+atm_config_temp+" "+atm_config
    os.system(command_cp)
    ###change contents
    command_sed = "sed -i 's|OSCILLATORCFG.*|OSCILLATORCFG = \""+oscillator_config+"\"|' "+atm_config
    os.system(command_sed)
    output_file=temp_dir+"/SystLLHScan_SKOnly_OscOnly_HPD_PHAoff_coarse"+oscillator_temp_coarse+"_fine"+oscillator_temp_fine+".root"
    command_sed = "sed -i 's|OUTPUTNAME.*|OUTPUTNAME = \""+output_file+"\"|' "+atm_config
    os.system(command_sed)
    #submit script
    job_temp = work_dir+"/job_template_run_SystLLHScan_SKOnly_oschpd.sh"
    job_subscript = temp_dir+"/job_run_SystLLHScan_SKOnly_oschpd.sh"
    command_cp = "cp "+job_temp+" "+job_subscript
    os.system(command_cp)
    output_log_dir = temp_dir+"/output_logs"
    command_mkdir = "mkdir -p "+output_log_dir
    os.system(command_mkdir)
    command_sed = "sed -i 's|OUTPUTLOG|"+output_log_dir+"|' "+job_subscript
    os.system(command_sed)
    command_sed = "sed -i 's|COARBIN|"+oscillator_temp_coarse+"|' "+job_subscript
    os.system(command_sed)
    command_sed = "sed -i 's|FINEBIN|"+oscillator_temp_fine+"|' "+job_subscript
    os.system(command_sed)
    command_sed = "sed -i 's|ATMCONFIG.*|"+atm_config+"|' "+job_subscript
    os.system(command_sed)

    print("DONE.")

if __name__ == "__main__" :
    main()
