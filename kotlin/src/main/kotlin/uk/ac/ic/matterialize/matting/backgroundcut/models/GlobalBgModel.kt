package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.TermCriteria
import org.opencv.ml.EM
import uk.ac.ic.matterialize.matting.backgroundcut.models.GMMGlobalColorModel

class GlobalBgModel(private val bgImage: FlatImage) {
    private val gmm = EM.create()
    // K_b in the background cut paper equation (2)
    // typical values in range 10-15
    private val numComponents = 10

    init {
        gmm.termCriteria = TermCriteria(
            gmm.termCriteria.type,
            10,
            gmm.termCriteria.epsilon
        )

        gmm.clustersNumber = numComponents
        // FIXME requires sample channels to be 1 so might need to convert image
        gmm.trainEM(bgImage.toSamples())
    }

    fun globalProbs(image: FlatImage): Probs {
        return GMMGlobalColorModel.globalProbs(gmm, image)
    }
}
