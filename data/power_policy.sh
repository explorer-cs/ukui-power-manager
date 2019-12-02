#!/bin/bash

power_status=$1
power_mode=$2

if [ $# -ne 2 ];
then
	echo "args error"
	exit
fi

echo ${power_status}
echo ${power_mode}

hosts=`ls /sys/class/scsi_host/`
policys=`ls /sys/devices/system/cpu/cpufreq/`
cards=`ls /sys/class/drm/`

function cpu_scaling_gover_speed()
{
	scaling_governor="performance"
	scaling_setspeed=1000000

	if [ ${power_status} == "AC" ];
	then
		scaling_governor="performance"
	fi

	if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
	then
		scaling_governor="userspace"
		scaling_setspeed=1000000
	fi

	for policy in ${policys};
	do
		if [ -f "/sys/devices/system/cpu/cpufreq/${policy}/scaling_governor" ];
		then
			echo ${scaling_governor} > /sys/devices/system/cpu/cpufreq/${policy}/scaling_governor
		fi
	
	        if [ ${power_status} == "BAT" -a -f "/sys/devices/system/cpu/cpufreq/${policy}/scaling_setspeed" ];
       		then
                	echo ${scaling_setspeed} > /sys/devices/system/cpu/cpufreq/${policy}/scaling_setspeed
        	fi
	done
}

function drm_set_mode()
{
	state="performance"
	level="auto"
	if [ ${power_status} == "AC" ];
	then
		state="performance"
	fi

	if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
	then
		state="boot"
		level="low"
	fi
	

	for card in ${cards};
	do
		if [ -f "/sys/class/drm/${card}/device/power_dpm_state" ];
		then
			echo ${state} > /sys/class/drm/${card}/device/power_dpm_state
		fi
	
		if [ -f "/sys/class/drm/${card}/device/power_dpm_force_performance_level" ];
		then
			echo ${level} > /sys/class/drm/${card}/device/power_dpm_force_performance_level
		fi
	done	
}

function pcie_aspm_set_mode()
{
	pcie_policy="performance"
	if [ ${power_status} == "AC" ];
	then
		pcie_policy="performance"
	fi
	
	if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
	then
		pcie_policy="powersupersave"
	fi
	if [ -f "/sys/module/pcie_aspm/parameters/policy" ];
	then
		echo ${pcie_policy} > /sys/module/pcie_aspm/parameters/policy
	fi	
}

function sata_alsm_set()
{
        alsm_policy="max_performance"
        if [ ${power_status} == "AC" ];
        then
                alsm_policy="max_performance"
        fi

        if [ ${power_status} == "BAT" -a ${power_mode} == "POWERSAVE" ];
        then
                alsm_policy="min_power"
        fi

        for host in ${hosts};
        do
                if [ -f "/sys/class/scsi_host/${host}/link_power_management_policy" ];
                then
			echo /sys/class/scsi_host/${host}/link_power_management_policy
                        echo ${alsm_policy} > /sys/class/scsi_host/${host}/link_power_management_policy
                fi
        done
}

cpu_scaling_gover_speed
drm_set_mode
pcie_aspm_set_mode
sata_alsm_set

