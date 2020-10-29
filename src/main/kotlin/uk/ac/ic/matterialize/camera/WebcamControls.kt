package uk.ac.ic.matterialize.camera

import uk.ac.ic.matterialize.util.runCommand

class WebcamControls(val deviceNumber: Int = 0) {
    private val device = "/dev/video$deviceNumber"
    private val timeout = 5

    fun disableAutomaticControls() {
        return
        "v4l2-ctl -d $device --set-ctrl=exposure_auto=1".runCommand()
        "v4l2-ctl -d $device --set-ctrl=white_balance_temperature_auto=0".runCommand()
    }

    fun enableAutomaticControls() {
        "v4l2-ctl -d $device --set-ctrl=exposure_auto=3".runCommand()
        "v4l2-ctl -d $device --set-ctrl=white_balance_temperature_auto=3".runCommand()
    }
}
