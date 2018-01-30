pipeline {
    agent any

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
                sh """ ./rbase-arm64 cmake -Bbuild -H. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -Dtesting=ON """
            }
        }
				stage('Build') {
				    steps {
                sh """ ./rbase-arm64 make -C build -j 2 """
            }
        }
				stage('Test') {
            steps {
						    sh """ make -C build test ARGS="-T Test" """
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
								sh """ rsync -avh  --no-perms --no-owner --no-group /var/lib/jenkins/.aptly/public/ /mnt/harptech-repos-deb/ --delete """
            }
        }
    }
}
