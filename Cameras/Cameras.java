public void robotInit() {
    	
    	Thread t = new Thread(() -> {
    		
    		boolean allowCam1 = false;
            boolean allowCam2;
            boolean allowCam3;
            boolean allowCam4;
   
    		UsbCamera camera1 = CameraServer.getInstance().startAutomaticCapture(0);
            camera1.setResolution(320, 240);
            camera1.setFPS(30);
            
            UsbCamera camera2 = CameraServer.getInstance().startAutomaticCapture(1);
            camera2.setResolution(320, 240);
            camera2.setFPS(30);
            
            UsbCamera camera3 = CameraServer.getInstance().startAutomaticCapture(2);
            camera3.setResolution(320, 240);
            
            UsbCamera camera4 = CameraServer.getInstance().startAutomaticCapture(3);
            camera4.setResolution(320, 240);
            
            CvSink cvSink1 = CameraServer.getInstance().getVideo(camera1);
            CvSink cvSink2 = CameraServer.getInstance().getVideo(camera2);
            CvSink cvSink3 = CameraServer.getInstance().getVideo(camera3);
            CvSink cvSink4 = CameraServer.getInstance().getVideo(camera4);
            CvSource outputStream = CameraServer.getInstance().putVideo("Switcher", 320, 240);
            
            Mat image = new Mat();
            
            while(!Thread.interrupted()) {
            	
            	if((Robot.oi.joySecondary.getRawButton(5)) {
            		allowCam1 = true;
                    allowCam2 = false;
                    allowCam3 = false;
                    allowCam4 = false;
            	}
                if((Robot.oi.joySecondary.getRawButton(6)) {
            		allowCam2 = true;
                    allowCam1 = false;
                    allowCam3 = false;
                    allowCam4 = false;
            	}
                if((Robot.oi.joySecondary.getRawButton(7)) {
            		allowCam3 = true;
                    allowCam1 = false;
                    allowCam2 = false;
                    allowCam4 = false;
            	}
                if((Robot.oi.joySecondary.getRawButton(8)) {
            		allowCam4 = true;
                    allowCam1 = false;
                    allowCam2 = false;
                    allowCam3 = false;
            	}
            	
                if(allowCam1){
                  cvSink2.setEnabled(false);
                  cvSink3.setEnabled(false);
                  cvSink4.setEnabled(false);
                  cvSink1.setEnabled(true);
                  cvSink1.grabFrame(image);
                } else if(allowCam2) {
                  cvSink1.setEnabled(false);
                  cvSink3.setEnabled(false);
                  cvSink4.setEnabled(false);
                  cvSink2.setEnabled(true);
                  cvSink2.grabFrame(image);     
                } else if(allowCam3) {
                  cvSink1.setEnabled(false);
                  cvSink2.setEnabled(false);
                  cvSink4.setEnabled(false);
                  cvSink3.setEnabled(true);
                  cvSink3.grabFrame(image);     
                }else if(allowCam4) {
                  cvSink1.setEnabled(false);
                  cvSink2.setEnabled(false);
                  cvSink3.setEnabled(false);
                  cvSink4.setEnabled(true);
                  cvSink4.grabFrame(image);     
                }
                
                outputStream.putFrame(image);
            }
            
        });
        t.start();
