pipeline {
    agent any

    options {
        buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '30'))
    }

    stages {
        stage('Configure') {
            steps {
                sh """ cmake -BbuildArm64 -H. -DCMAKE_BUILD_TYPE=Release -Dtesting=ON -DBUILD_NUMBER=$BUILD_NUMBER -DCMAKE_TOOLCHAIN_FILE=RCore/AArch64Toolchain.cmake """
                sh """ chmod +x ./buildArm64/build-cli.sh """

                sh """ cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release -Dtesting=ON -DBUILD_NUMBER=$BUILD_NUMBER """
                sh """ chmod +x ./buildArm64/build-cli.sh """
            }
        }
        stage('Build') {
            steps {
                sh """ touch RCore/AArch64Toolchain.cmake """
                sh """ ./buildArm64/build-cli.sh make -C buildArm64 -j 2 """
                sh """ make -C build -j 2 """
            }
        }
        stage('Test') {
            steps {
                sh """ ./build/test-runner -r junit -o ./build/junit.xml """
            }
        }
        stage('Package') {
            steps {
                sh """ chmod +x ./buildArm64/build-cli.sh """
                sh """ ./buildArm64/build-cli.sh make -C buildArm64 package """
            }
        }
        stage('Publish') {
            steps {
                sh """ aptly repo add harptech-testing ./buildArm64/packages/*.deb """
                sh """ aptly publish update xenial """
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'buildArm64/packages/*.deb', fingerprint: true
            junit 'build/junit.xml'
        }
    }
}
