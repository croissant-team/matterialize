package uk.ac.ic.matterialize.camera

import uk.ac.ic.matterialize.util.runCommand

class WebcamControls(deviceNumber: Int = 0) {
    private val device = "/dev/video$deviceNumber"
    private val timeout = 5

    init {
        enableAutomaticControls()
    }

    fun disableAutomaticControls() {
        println("Disabling automatic webcam controls")
        "v4l2-ctl -d $device --set-ctrl=exposure_auto=1".runCommand()
        "v4l2-ctl -d $device --set-ctrl=white_balance_temperature_auto=0".runCommand()
    }

    fun enableAutomaticControls() {
        println("Enabling automatic webcam controls")
        "v4l2-ctl -d $device --set-ctrl=exposure_auto=3".runCommand()
        "v4l2-ctl -d $device --set-ctrl=white_balance_temperature_auto=1".runCommand()
    }
}
