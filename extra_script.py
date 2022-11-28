Import("env")

# Upload actions
#
def before_build(source, target, env):
    print("************ before_build")
    # do some actions

def after_build(source, target, env):
    print("***************after_build")
    # do some actions

def copy2home(source, target, env):
    print("********** Copy firmware.bin to home/jg_heizung.bin ************")
    env.Execute("cp .pio/build/esp32dev/firmware.bin /home/juergen/jg_heizung.bin")
    print("****************************************************************")

#env.AddPreAction("buildprog", before_build)
#env.AddPostAction("buildprog", after_build)

# by JG Copy file to home dir
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin",copy2home)