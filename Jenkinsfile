pipeline {
    agent any

    stages {
        stage('Docker Setup') {
            steps {
                sh """ docker build -t rbase-arm64 . """
								sh """ docker run --rm rbase-arm64 > ./rbase-arm64 """
								sh """    chmod +x ./rbase-arm64 """
            }
        }
				stage('Configure') {
				    steps {
                sh """ ./rbase-arm64 cmake -Bbuild -H. -G"Unix Makefiles" """
            }
        }
				stage('Build') {
				    steps {
                sh """ ./rbase-arm64 make -C build -j 2 """
            }
        }
				stage('Package') {
				    steps {
                sh """ ./rbase-arm64 make -C build package -j 2 """
            }
        }
    }
}
