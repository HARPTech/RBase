pipeline {
    agent any

    options {
        buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '30'))
    }

    stages {
        stage('Docker Setup') {
            steps {
                sh """ docker build -t rbase-arm64 . """
								sh """ docker run --rm rbase-arm64 > ./rbase-arm64 """
								sh """ chmod +x ./rbase-arm64 """
            }
        }
				stage('Configure') {
				    steps {
                sh """ ./rbase-arm64 cmake -Bbuild -H. -G'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -Dtesting=ON -DBUILD_NUMBER=$BUILD_NUMBER """
            }
        }
				stage('Build') {
				    steps {
                sh """ ./rbase-arm64 make -C build -j 2 """
            }
        }
				stage('Test') {
            steps {
						    sh """ ./rbase-arm64 make -C build test ARGS="-T Test" """
						}
        }
				stage('Package') {
				    steps {
                sh """ ./rbase-arm64 make -C build package -j 2 """
            }
        }
				stage('Publish') {
            steps {
                sh """ aptly repo add harptech-testing ./packages/*.deb """
								sh """ aptly publish update jessie testing """
            }
        }
    }

    post {
        always {
            junit 'build/*/*/report.xml'
				}
    }
}
