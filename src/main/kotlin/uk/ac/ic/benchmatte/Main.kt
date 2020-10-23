package uk.ac.ic.benchmatte

import uk.ac.ic.benchmatte.util.Benchmark

fun main() {
    nu.pattern.OpenCV.loadLocally()

    val benchmark = Benchmark(
        "/home/line/usr/doc/ic/assignments/3xx_60021_segp/adobe/Combined_Dataset/Training_set/Other/alpha/3262986095_2d5afe583c_b.jpg",
        "/home/line/usr/doc/ic/assignments/3xx_60021_segp/adobe/bgs/train2014/COCO_train2014_000000000762.jpg",
        "/home/line/usr/doc/ic/assignments/3xx_60021_segp/adobe/Combined_Dataset/Training_set/Other/fg/3262986095_2d5afe583c_b.jpg"
    )

    benchmark.setup()
    benchmark.export()

    println(benchmark.run())
}
