import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    kotlin("jvm") version "1.4.10"
    id("org.jlleitschuh.gradle.ktlint") version "9.4.1"
    id("org.openjfx.javafxplugin") version "0.0.9"

}

group = "uk.ic.ac.matterialize"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(kotlin("test-junit"))
    implementation("org.openpnp:opencv:4.3.0-2")
    implementation("com.github.haifengl:smile-kotlin:2.5.3")
    implementation("no.tornado:tornadofx:1.7.20")
}


javafx {
    modules("javafx.controls", "javafx.fxml", "javafx.swing")
}

tasks.withType<KotlinCompile>() {
    kotlinOptions.jvmTarget = "1.8"
}
