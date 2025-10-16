import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

// 全局变量
let scene, camera, renderer, controls, model, mixer;
let lastUpdateTime = 0;
let updateCount = 0;
let dataHistory = {
  left: Array(50).fill(null),
  right: Array(50).fill(null),
  distance: Array(50).fill(null),
  timestamps: Array(50).fill(null)
};
let chart;

// 初始化3D场景
function initThreeJS() {
  // 1. 场景 + 相机 + 渲染器
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0x1D2129);

  camera = new THREE.PerspectiveCamera(
    45,
    document.getElementById('model-canvas').clientWidth / document.getElementById('model-canvas').clientHeight,
    0.1,
    100
  );

  const distance = 3.5;        // 与原点保持 4 单位距离，视角略微向下倾斜一点
  const angle  = - Math.PI / 6;
  const height = 1;        

  camera.position.set(
    distance * Math.sin(angle), // +X 方向右侧
    height,                     // +Y 上方
    distance * Math.cos(angle)  // +Z 前方
  );
  // controls.target.set(0, 0, 0); // 依旧看向模型中心

  renderer = new THREE.WebGLRenderer({ 
    antialias: true,
    canvas: document.getElementById('model-canvas')
  });
  renderer.setSize(
    document.getElementById('model-canvas').clientWidth,
    document.getElementById('model-canvas').clientHeight
  );
  renderer.shadowMap.enabled = true;

  // 2. 轨道控制器
  controls = new OrbitControls(camera, renderer.domElement);
  controls.enableDamping = true;
  controls.dampingFactor = 0.05;

  // 3. 灯光
  const hemLight = new THREE.HemisphereLight(0xffffff, 0x444444, 5);
  scene.add(hemLight);

  const dirLight = new THREE.DirectionalLight(0xffffff, 4);
  dirLight.position.set(5, 10, 7);
  dirLight.castShadow = true;
  dirLight.shadow.mapSize.width = 2048;
  dirLight.shadow.mapSize.height = 2048;
  scene.add(dirLight);

  const backLight = new THREE.DirectionalLight(0xffffff, 0.8);
  backLight.position.set(-5, -5, -5);
  scene.add(backLight);
  // 4. 添加地面
  const groundGeometry = new THREE.PlaneGeometry(8, 8);
  const groundMaterial = new THREE.MeshStandardMaterial({
    color: 0x1a1a1a,     // 深色背景融合
    roughness: 0.2,      // 可以更光滑一些
    metalness: 0.1,      // 略带金属光泽，提升精致感
    transparent: true,   // 开启透明
    opacity: 0.8         // 80% 不透明，仍有轻微透明感
  });
  const ground = new THREE.Mesh(groundGeometry, groundMaterial);
  ground.rotation.x = -Math.PI / 2;
  ground.position.y = -0.8; 
  ground.receiveShadow = true;
  scene.add(ground);

  // 5. 加载模型
  const loader = new GLTFLoader();
  loader.load(
    './model/sherioc_car.glb',
    gltf => {
      model = gltf.scene;
      scene.add(model);

      // 模型居中并缩放
      const box = new THREE.Box3().setFromObject(model);
      const center = box.getCenter(new THREE.Vector3());
      const size = box.getSize(new THREE.Vector3());
      const maxDim = Math.max(size.x, size.y, size.z);
      const scale = 3 / maxDim;
      model.scale.multiplyScalar(scale);
      model.position.sub(center.multiplyScalar(scale));

      // 启用阴影
      model.traverse(child => {
        if (child.isMesh) {
          child.castShadow = true;
          child.receiveShadow = true;
        }
      });

      // 动画处理
      if (gltf.animations.length) {
        mixer = new THREE.AnimationMixer(model);
        gltf.animations.forEach(clip => mixer.clipAction(clip).play());
      }

      document.getElementById('loading-indicator').classList.add('hidden');
      document.getElementById('model-status').textContent = '已加载';
    },
    xhr => console.log((xhr.loaded / xhr.total * 100) + '% loaded'),
    err => {
      console.error(err);
      document.getElementById('loading-indicator').innerHTML = `
        <div class="flex flex-col items-center">
          <i class="fa fa-exclamation-triangle text-danger text-2xl"></i>
          <p class="mt-2 text-sm text-slate-400">模型加载失败</p>
        </div>
      `;
    }
  );

  // 6. 动画循环
  function animate() {
    requestAnimationFrame(animate);
    controls.update();
    if (mixer) mixer.update(0.016);
    renderer.render(scene, camera);
  }
  animate();

  // 7. 窗口大小调整
  window.addEventListener('resize', () => {
    const canvas = document.getElementById('model-canvas');
    camera.aspect = canvas.clientWidth / canvas.clientHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(canvas.clientWidth, canvas.clientHeight);
  });

  // 8. 视图控制
  document.getElementById('reset-view').addEventListener('click', () => {
    camera.position.set(3, 2, 3);
    controls.reset();
  });

  document.getElementById('fullscreen').addEventListener('click', () => {
    const canvas = document.getElementById('model-canvas');
    if (!document.fullscreenElement) {
      canvas.requestFullscreen().catch(err => {
        console.error(`全屏请求失败: ${err.message}`);
      });
    } else {
      document.exitFullscreen();
    }
  });
}

// 初始化图表
function initChart() {
  const ctx = document.getElementById('data-chart').getContext('2d');
  chart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: dataHistory.timestamps,
      datasets: [
        {
          label: '左侧速度',
          data: dataHistory.left,
          borderColor: '#165DFF',
          backgroundColor: 'rgba(22, 93, 255, 0.1)',
          tension: 0.4,
          fill: false
        },
        {
          label: '右侧速度',
          data: dataHistory.right,
          borderColor: '#36CFC9',
          backgroundColor: 'rgba(54, 207, 201, 0.1)',
          tension: 0.4,
          fill: false
        },
        {
          label: '距离 (cm)',
          data: dataHistory.distance,
          borderColor: '#52C41A',
          backgroundColor: 'rgba(82, 196, 26, 0.1)',
          tension: 0.4,
          fill: false,
          yAxisID: 'y1'
        }
      ]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          grid: {
            color: 'rgba(255, 255, 255, 0.05)'
          },
          ticks: {
            color: 'rgba(255, 255, 255, 0.7)'
          }
        },
        y: {
          min: -100,
          max: 100,
          grid: {
            color: 'rgba(255, 255, 255, 0.05)'
          },
          ticks: {
            color: 'rgba(255, 255, 255, 0.7)'
          },
          title: {
            display: true,
            text: '速度',
            color: 'rgba(255, 255, 255, 0.7)'
          }
        },
        y1: {
          position: 'right',
          min: 0,
          max: 100,
          grid: {
            drawOnChartArea: false
          },
          ticks: {
            color: 'rgba(255, 255, 255, 0.7)'
          },
          title: {
            display: true,
            text: '距离 (cm)',
            color: 'rgba(255, 255, 255, 0.7)'
          }
        }
      },
      plugins: {
        legend: {
          labels: {
            color: 'rgba(255, 255, 255, 0.7)'
          }
        }
      }
    }
  });
}

// 处理WebSocket数据
function initWebSocket() {
  const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
  const wsUri = `${protocol}//${window.location.host}/ws`;
  const websocket = new WebSocket(wsUri);

  websocket.onopen = () => {
    console.log('WebSocket连接已建立');
    document.getElementById('connection-status').className = 'w-2 h-2 rounded-full bg-success mr-2';
    document.getElementById('connection-text').textContent = '已连接';
    lastUpdateTime = Date.now();
    updateCount = 0;
  };

  websocket.onclose = () => {
    console.log('WebSocket连接已关闭');
    document.getElementById('connection-status').className = 'w-2 h-2 rounded-full bg-danger mr-2';
    document.getElementById('connection-text').textContent = '已断开';
    
    // 尝试重连
    setTimeout(initWebSocket, 3000);
  };

  websocket.onmessage = (e) => {
    try {
      const data = JSON.parse(e.data);
      updateUI(data);
      updateDataHistory(data);
      updateChart();
      updateRateCounter();
    } catch (error) {
      console.error('解析数据失败:', error);
    }
  };

  websocket.onerror = (error) => {
    console.error('WebSocket错误:', error);
  };
}

// 更新UI显示
function updateUI(data) {
  // 更新速度显示
  document.getElementById('left-speed').textContent = data.left;
  document.getElementById('right-speed').textContent = data.right;
  document.getElementById('distance').textContent = data.dist + ' cm';

  // 更新进度条
  const leftPercent = (data.left + 100) / 2; // 将-100~100映射到0~100
  const rightPercent = (data.right + 100) / 2;
  const distancePercent = Math.min(100, data.dist); // 距离最大100cm
  
  document.getElementById('left-speed-bar').style.width = `${leftPercent}%`;
  document.getElementById('right-speed-bar').style.width = `${rightPercent}%`;
  document.getElementById('distance-bar').style.width = `${distancePercent}%`;

  // 根据速度旋转车轮（如果模型有车轮且命名规范）
  if (model) {
    // 这里假设模型中有命名为"wheel_left"和"wheel_right"的车轮 mesh
    const leftWheel = model.getObjectByName('wheel_left');
    const rightWheel = model.getObjectByName('wheel_right');
    
    if (leftWheel) leftWheel.rotation.x += data.left * 0.01;
    if (rightWheel) rightWheel.rotation.x += data.right * 0.01;
  }
}

// 更新数据历史记录
function updateDataHistory(data) {
  const now = new Date();
  const timeStr = `${now.getMinutes().toString().padStart(2, '0')}:${now.getSeconds().toString().padStart(2, '0')}`;
  
  // 移除最旧的数据，添加新数据
  dataHistory.left.shift();
  dataHistory.right.shift();
  dataHistory.distance.shift();
  dataHistory.timestamps.shift();
  
  dataHistory.left.push(data.left);
  dataHistory.right.push(data.right);
  dataHistory.distance.push(data.dist);
  dataHistory.timestamps.push(timeStr);
}

// 更新图表
function updateChart() {
  if (chart) {
    chart.data.labels = dataHistory.timestamps;
    chart.data.datasets[0].data = dataHistory.left;
    chart.data.datasets[1].data = dataHistory.right;
    chart.data.datasets[2].data = dataHistory.distance;
    chart.update();
  }
}

// 更新数据刷新率
function updateRateCounter() {
  updateCount++;
  const now = Date.now();
  const elapsed = (now - lastUpdateTime) / 1000;
  
  if (elapsed >= 1) {
    const rate = Math.round(updateCount / elapsed);
    document.getElementById('update-rate').textContent = `${rate} Hz`;
    updateCount = 0;
    lastUpdateTime = now;
  }
}

// 初始化应用
function initApp() {
  initThreeJS();
  initChart();
  initWebSocket();
}

// 页面加载完成后初始化
window.addEventListener('load', initApp);