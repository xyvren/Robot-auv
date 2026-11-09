#include "web_pages.h"

namespace ROVWebPages {

const char CONTROL_PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="id"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>SAUVAC | ROV Control</title>
<style>
*{box-sizing:border-box}body{--bg:#0b1118;--surface:#121d28;--surface-raised:#182633;--line:#263846;--text:#edf5fa;--muted:#a9bac7;--cyan:#5ed8e8;--green:#73d6a0;margin:0;background:var(--bg);color:var(--text);font:15px/1.5 system-ui,-apple-system,"Segoe UI",sans-serif}
button,select,input{font:inherit}button{cursor:pointer;min-height:44px}
.skip-link{position:fixed;top:-60px;left:12px;z-index:20;padding:10px 14px;background:#fff;color:#111;border-radius:6px}.skip-link:focus{top:12px}
.topbar{height:68px;display:flex;align-items:center;gap:16px;padding:0 24px;background:#101923;border-bottom:1px solid var(--line);position:sticky;top:0;z-index:5}
.brand{display:flex;align-items:center;gap:11px;min-width:220px}.brand-mark{width:34px;height:34px;display:grid;place-items:center;border:1px solid var(--cyan);border-radius:8px;color:var(--cyan);font-weight:800;letter-spacing:-1px}
.brand-name{font-weight:800;letter-spacing:.13em;font-size:1.04rem}.brand-caption{display:block;color:var(--muted);font:10px/1.2 ui-monospace,monospace;letter-spacing:.12em}
.menu-toggle{display:grid;place-items:center;width:44px;padding:0;border:1px solid var(--line);border-radius:8px;background:var(--surface);color:var(--text)}
.menu-toggle svg{width:21px;height:21px}.topbar-status{margin-left:auto;display:flex;align-items:center;gap:9px;color:var(--muted);font-size:.9rem}
.state-dot{width:8px;height:8px;border-radius:50%;background:#e5a84b;box-shadow:0 0 0 3px #e5a84b22}
.layout{display:grid;grid-template-columns:244px minmax(0,1fr);min-height:calc(100vh - 68px);transition:grid-template-columns .2s ease}
.sidebar{position:sticky;top:68px;align-self:start;height:calc(100vh - 68px);padding:24px 14px;background:#101923;border-right:1px solid var(--line);display:flex;flex-direction:column;z-index:4}
.nav-caption{padding:0 12px 10px;color:#8295a4;font:10px ui-monospace,monospace;letter-spacing:.15em}
.nav-list{display:grid;gap:5px}.nav-link{display:flex;align-items:center;gap:13px;width:100%;padding:10px 12px;border:1px solid transparent;border-radius:8px;background:transparent;color:#b5c5d0;text-align:left;white-space:nowrap}
.nav-link svg{width:20px;height:20px;flex:none;stroke:currentColor;fill:none;stroke-width:1.7;stroke-linecap:round;stroke-linejoin:round}
.nav-link:hover{background:#172531;color:var(--text)}.nav-link[aria-current="page"]{background:#17313d;border-color:#244b58;color:#8ce5ee}
.nav-link:focus-visible,.menu-toggle:focus-visible,button:focus-visible,input:focus-visible,select:focus-visible{outline:2px solid var(--cyan);outline-offset:2px}
.sidebar-footer{margin-top:auto;border-top:1px solid var(--line);padding:16px 10px 4px;color:var(--muted);font-size:.85rem}
.sidebar-footer strong{display:block;color:var(--text);font-size:.9rem}.sidebar-footer span{display:flex;align-items:center;gap:8px}
.content{min-width:0;width:100%;max-width:1440px;margin:0 auto;padding:32px clamp(16px,3vw,42px) 48px}
.page-heading{margin-bottom:24px}.page-eyebrow{color:var(--cyan);font:11px ui-monospace,monospace;letter-spacing:.16em}.page-heading h1{margin:5px 0 2px;font-size:clamp(1.65rem,3vw,2.1rem);letter-spacing:-.03em}.page-heading p{margin:0;color:var(--muted)}
body.sidebar-collapsed .layout{grid-template-columns:76px minmax(0,1fr)}body.sidebar-collapsed .sidebar{padding-inline:10px}
body.sidebar-collapsed .nav-caption,body.sidebar-collapsed .nav-label,body.sidebar-collapsed .sidebar-footer .footer-label{display:none}
body.sidebar-collapsed .nav-link{justify-content:center;padding-inline:8px}body.sidebar-collapsed .sidebar-footer{padding-inline:8px}
section{background:var(--surface);border:1px solid var(--line);padding:20px;margin:14px 0;border-radius:12px}
h2{margin:0 0 8px;color:#dceaf2;font-size:1.15rem}section h2{color:#dceaf2}
.panel{display:none}.panel.active{display:block}
.axis{display:grid;grid-template-columns:120px 1fr 48px;gap:8px;align-items:center}
input[type=range]{width:100%}
.settings-intro{max-width:760px;margin:0 0 16px;color:var(--muted)}
.settings-group{margin:18px 0 10px;padding-top:14px;border-top:1px solid var(--line)}
.settings-group h3{margin:0 0 4px;font-size:1rem}
.settings-fields{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:12px;margin:14px 0}
.settings-fields label,.motor-fields label{display:grid;gap:5px;color:var(--text);font-size:.9rem}
.settings-fields input{width:100%}
.mixer-help{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:10px;margin:12px 0}
.mixer-help div{padding:10px 12px;border:1px solid var(--line);border-radius:8px;background:#fff}
.mixer-help dt{font-weight:700;color:var(--text)}
.mixer-help dd{margin:3px 0 0;color:var(--muted);font-size:.88rem}
.config-message{display:flex;align-items:center;justify-content:space-between;gap:12px;margin:12px 0;padding:12px 14px;border:1px solid #c7d3de;border-radius:8px;background:#f8fafc;color:#40566a}
.config-message[hidden]{display:none}
.config-message.error{border-color:#e4b7b7;background:#fff2f2;color:#8f2020}
.config-message button{min-height:44px;padding:8px 12px;border:1px solid #b9d1da;border-radius:7px;background:#fff;color:#174454}
.motor{margin:12px 0;padding:16px;border:1px solid var(--line);border-radius:10px;background:#f8fafc}
.motor h4{margin:0 0 12px;color:var(--text);font-size:1rem}
.motor-fields{display:grid;grid-template-columns:repeat(auto-fit,minmax(145px,1fr));gap:10px;margin-bottom:12px}
.motor-fields input:not([type=checkbox]),.motor-fields select{width:100%;min-width:0}
.motor-fields .reverse-label{display:flex;align-items:center;gap:8px;min-height:44px}
.matrix-wrap{min-width:0;overflow-x:auto;padding-bottom:4px}
.matrix{display:grid;grid-template-columns:repeat(7,minmax(58px,1fr));gap:5px;min-width:460px}
.matrix b{align-self:center;font-size:.8rem}
.matrix input{width:100%;min-height:40px;padding:6px}
.small{font-size:.9em;color:var(--muted)}
.quick-controls{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:12px;margin:16px 0}
.quick-card{padding:14px;background:#0e1720;border:1px solid var(--line);border-radius:9px}
.quick-card h3{margin:0 0 10px;font-size:.95rem;color:#dceaf2}
.quick-actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}
.quick-button{min-height:48px;padding:8px;border:1px solid #385363;border-radius:8px;background:#152733;color:var(--text);touch-action:none;user-select:none}
.quick-button:hover,.quick-button:focus-visible{background:#1b3c4b}
.quick-button[aria-pressed="true"]{background:#176187;border-color:var(--cyan);color:#fff}
.quick-strength{display:flex;align-items:center;gap:12px;max-width:440px;padding:8px 0}
.quick-strength input{flex:1}
.quick-note{min-height:24px;color:var(--muted)}
.individual-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:10px;margin:14px 0}
.individual-card{padding:14px;background:#f8fafc;border:1px solid var(--line);border-radius:9px}
.individual-card h4{margin:0 0 8px;font-size:.95rem}
.individual-reading{display:block;margin-bottom:10px;color:var(--muted);font:13px ui-monospace,monospace}
.individual-actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}
.individual-button{min-height:48px;padding:8px;border:1px solid #9aafbd;border-radius:8px;background:#fff;color:var(--text);touch-action:none;user-select:none}
.individual-button:hover,.individual-button:focus-visible{background:#e8f4f7}
.individual-button[aria-pressed="true"]{background:#087e9a;border-color:#087e9a;color:#fff}
#status{font-weight:600}.danger{background:#9e3b40;color:white;border:1px solid #c1585c;border-radius:7px}.danger:hover{background:#b4474d}
#stop,#save{border:1px solid #426273;border-radius:7px;background:#17313d;color:var(--text)}
.telemetry{display:grid;grid-template-columns:repeat(auto-fit,minmax(190px,1fr));gap:10px}
.reading{background:#0e1720;border:1px solid var(--line);border-radius:9px;padding:14px}
.reading b{display:block;color:#9bb2c1;margin-bottom:6px;font-size:.84rem;font-weight:600}.reading output{font:1.3rem ui-monospace,monospace;color:#eaf5fa}
.good{color:var(--green)}.bad{color:#ff9b9b}
.backdrop{display:none}
@media(max-width:760px){
  .topbar{height:60px;padding:0 14px;gap:12px}.brand{min-width:0}.brand-caption{display:none}.topbar-status{font-size:.78rem}
  .layout,body.sidebar-collapsed .layout{display:block;min-height:calc(100vh - 60px)}
  .sidebar,body.sidebar-collapsed .sidebar{position:fixed;top:60px;bottom:0;left:0;width:min(280px,84vw);height:auto;padding:20px 14px;transform:translateX(-105%);transition:transform .2s ease;z-index:7;box-shadow:14px 0 40px #0005}
  body.sidebar-open .sidebar{transform:translateX(0)}
  body.sidebar-collapsed .nav-caption,body.sidebar-collapsed .nav-label,body.sidebar-collapsed .sidebar-footer .footer-label{display:block}
  body.sidebar-collapsed .nav-link{justify-content:flex-start;padding:10px 12px}body.sidebar-collapsed .sidebar-footer{padding:16px 10px 4px}
  .backdrop{position:fixed;inset:60px 0 0;background:#0009;z-index:6}body.sidebar-open .backdrop{display:block}
  .content{padding:22px 14px 36px}section{padding:16px;margin:12px 0}
  .quick-controls{grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}.quick-card{padding:11px}
  .individual-grid{grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}.individual-card{padding:11px}
}
@media(max-width:520px){.topbar-status{max-width:48%;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}.axis{grid-template-columns:95px 1fr 42px}}
@media(max-width:380px){.quick-controls,.individual-grid{grid-template-columns:1fr}}
@media(prefers-reduced-motion:reduce){*,*::before,*::after{scroll-behavior:auto!important;transition:none!important}}
body{--bg:#f4f7fb;--surface:#fff;--surface-raised:#f8fafc;--line:#dce4ec;--text:#172b3a;--muted:#526678;--cyan:#087e9a;--green:#16794b;background:var(--bg);color:var(--text)}
.topbar,.sidebar{background:#fff;border-color:#dce4ec}
.brand-mark{background:#e8f7fa;color:#087e9a}
.menu-toggle{background:#fff;color:#172b3a;border-color:#cdd8e2}
.topbar-status,.page-heading p,.small,.sidebar-footer{color:#526678}
.nav-caption{color:#65798b}
.nav-link{color:#40566a}
.nav-link:hover{background:#f1f6fa;color:#172b3a}
.nav-link[aria-current="page"]{background:#e7f5f8;border-color:#b9e2e9;color:#08677e}
.nav-link:focus-visible,.menu-toggle:focus-visible,button:focus-visible,input:focus-visible,select:focus-visible{outline-color:#087e9a}
.page-eyebrow{color:#087e9a}
section{background:#fff;border-color:#dce4ec;box-shadow:0 2px 8px #18344b08}
h2,section h2,.quick-card h3{color:#172b3a}
h3{color:#172b3a}
.settings-group{border-color:#dce4ec}
.config-message{background:#f8fafc;border-color:#dce4ec;color:#40566a}
.config-message.error{background:#fff2f2;border-color:#e4b7b7;color:#8f2020}
.mixer-help div{background:#fff;border-color:#dce4ec}
.motor{background:#f8fafc;border-color:#dce4ec}
.reading,.quick-card{background:#f8fafc;border-color:#dce4ec}
.reading b{color:#526678}
.reading output{color:#172b3a}
.quick-button{background:#fff;border-color:#c5d3df;color:#172b3a}
.quick-button:hover,.quick-button:focus-visible{background:#edf6f9}
.quick-button[aria-pressed="true"]{background:#087e9a;border-color:#087e9a;color:#fff}
#stop,#save{background:#eaf4f7;border-color:#b9d1da;color:#174454}
.danger{background:#b4232f;border-color:#a51d28}.danger:hover{background:#941b24}
input:not([type=range]),select{min-height:40px;padding:7px 10px;border:1px solid #c7d3de;border-radius:7px;background:#fff;color:#172b3a}
input[type=range]{accent-color:#087e9a}
.motor{border-color:#e1e8ef}
.chart-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:14px;margin-top:16px}
.chart-card{min-width:0;padding:16px;background:#fff;border:1px solid #dce4ec;border-radius:11px}
.chart-heading{display:flex;align-items:flex-start;justify-content:space-between;gap:12px}
.chart-heading h3{margin:0;color:#172b3a;font-size:1rem}
.chart-heading p,.chart-note{margin:3px 0 0;color:#526678;font-size:.86rem}
.chart-window{padding:3px 8px;border:1px solid #dce4ec;border-radius:99px;color:#526678;font-size:.75rem;white-space:nowrap}
.chart-card canvas{display:block;width:100%;height:190px;margin-top:8px}
.chart-legend{display:flex;flex-wrap:wrap;gap:16px;color:#40566a;font-size:.83rem}
.chart-legend span{display:inline-flex;align-items:center;gap:6px}
.legend-line{width:17px;height:3px;border-radius:3px;background:#2878c7}
.roll-line,.gyro-y-line{background:#198754}.gyro-x-line{background:#d97706}.gyro-z-line{background:#087e9a}
.pressure-line{background:#2878c7}
.chart-note{margin:8px 2px 0}
#depthZero{min-height:44px;margin-top:8px;padding:8px 12px;border:1px solid #b9d1da;border-radius:7px;background:#eaf4f7;color:#174454}
#depthZero:disabled{opacity:.55;cursor:not-allowed}
@media(max-width:900px){.chart-grid{grid-template-columns:1fr}}
</style>
<body>
<a class="skip-link" href="#main-content">Lewati ke konten utama</a>
<header class="topbar">
  <button id="menuToggle" class="menu-toggle" type="button" aria-label="Buka menu navigasi" aria-controls="sidebar" aria-expanded="false">
    <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 6h16M4 12h16M4 18h16" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>
  </button>
  <div class="brand"><span class="brand-mark" aria-hidden="true">S</span><div><span class="brand-name">SAUVAC</span><span class="brand-caption">UNDERWATER ROBOTICS</span></div></div>
  <div class="topbar-status"><span class="state-dot" aria-hidden="true"></span><span id="status" aria-live="polite">Memuat status...</span></div>
</header>
<div class="layout">
<aside id="sidebar" class="sidebar" aria-label="Navigasi utama">
  <div class="nav-caption">PANEL KENDALI</div>
  <nav class="nav-list" aria-label="Menu SAUVAC">
    <button id="tab-monitor" class="nav-link" type="button" data-panel="monitor" aria-label="Monitoring" title="Monitoring" aria-controls="panel-monitor" aria-current="page">
      <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M3 12h3l2.2-6 4.2 12 2.2-6H21"/></svg><span class="nav-label">Monitoring</span>
    </button>
    <button id="tab-control" class="nav-link" type="button" data-panel="control" aria-label="Kontrol" title="Kontrol" aria-controls="panel-control">
      <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M12 3v18M3 12h18"/><circle cx="12" cy="12" r="7"/></svg><span class="nav-label">Kontrol</span>
    </button>
    <button id="tab-settings" class="nav-link" type="button" data-panel="settings" aria-label="Pengaturan" title="Pengaturan" aria-controls="panel-settings">
      <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M12 8.5a3.5 3.5 0 1 0 0 7 3.5 3.5 0 0 0 0-7Z"/><path d="m19.4 15 .1.1 1.4 1.1-1.4 2.4-1.7-.7a8 8 0 0 1-1.5.9l-.3 1.8h-2.8l-.3-1.8a8 8 0 0 1-1.5-.9l-1.7.7-1.4-2.4 1.4-1.1a7 7 0 0 1 0-1.8l-1.4-1.1 1.4-2.4 1.7.7a8 8 0 0 1 1.5-.9l.3-1.8h2.8l.3 1.8a8 8 0 0 1 1.5.9l1.7-.7 1.4 2.4-1.4 1.1a7 7 0 0 1 0 1.7Z" transform="translate(-1 -1) scale(.96)"/></svg><span class="nav-label">Pengaturan</span>
    </button>
  </nav>
  <div class="sidebar-footer"><span><span class="state-dot" aria-hidden="true"></span><strong class="footer-label">ROV Pico W</strong></span><span class="footer-label">6 thruster · sistem lokal</span></div>
</aside>
<button id="sidebarBackdrop" class="backdrop" type="button" aria-label="Tutup menu"></button>
<main id="main-content" class="content">
<div class="page-heading"><div id="pageEyebrow" class="page-eyebrow">SYSTEM OVERVIEW</div><h1 id="pageTitle">Monitoring</h1><p id="pageDescription">Status sensor dan telemetri SAUVAC.</p></div>
<section id="panel-monitor" class="panel active" aria-labelledby="tab-monitor">
  <h2>Monitoring navigasi</h2>
  <p class="small">Telemetri IMU dan kompas. Heading magnetometer belum dikalibrasi atau dikoreksi terhadap kemiringan; pembacaan dapat terganggu motor, kabel, dan logam.</p>
  <div class="telemetry">
    <div class="reading"><b>MPU-6050</b><output id="imuState">Memeriksa...</output><div class="small">Status sensor IMU</div></div>
    <div class="reading"><b>GY-271</b><output id="compassState">Memeriksa...</output><div class="small">Status sensor kompas</div></div>
    <div class="reading"><b>Heading kompas</b><output id="heading">--°</output><div class="small">Arah magnetik relatif sensor</div></div>
    <div class="reading"><b>Pitch / Roll</b><output id="attitude">--° / --°</output><div class="small">Estimasi kemiringan dari akselerometer + giroskop</div></div>
    <div class="reading"><b>Magnetometer X / Y / Z</b><output id="magnetic">-- / -- / --</output><div class="small">Raw, satuan sensor</div></div>
    <div class="reading"><b>Akselerometer X / Y / Z</b><output id="acceleration">-- / -- / --</output><div class="small">g</div></div>
    <div class="reading"><b>Giroskop X / Y / Z</b><output id="rotation">-- / -- / --</output><div class="small">°/s</div></div>
    <div class="reading"><b>MS5803-14BA</b><output id="pressureState">Memeriksa...</output><div class="small">Tekanan absolut · suhu kompensasi</div></div>
    <div class="reading"><b>Kedalaman relatif</b><output id="depthValue">-- m</output><div id="depthStatus" class="small">Nolkan sensor di permukaan air sebelum menyelam.</div><button id="depthZero" type="button" disabled>Nolkan di permukaan</button></div>
    <div class="reading"><b>Tegangan suplai Pico (VSYS)</b><output id="vsysValue">-- V</output><div id="vsysStatus" class="small">Memeriksa tegangan masuk ke papan Pico.</div></div>
    <div class="reading"><b>Sistem</b><output id="systemState">Memeriksa...</output><div class="small">Thruster, kontrol, dan waktu hidup</div></div>
  </div>
  <p class="chart-note">VSYS menunjukkan tegangan suplai yang masuk ke Pico, bukan selalu tegangan baterai utama. Jika Pico diberi daya dari baterai 12 V melalui buck converter, nilai ini adalah output buck. Jangan pernah sambungkan 12 V langsung ke VSYS; monitor baterai utama memerlukan rangkaian ukur tersendiri.</p>
  <p class="chart-note"><strong>Posisi awal IMU:</strong> sebelum menyalakan Pico, letakkan ROV diam pada permukaan datar dan rata, dalam posisi badan yang dianggap level saat operasi. Pasang modul MPU-6050 dengan kuat; sejajarkan sumbu X modul dengan arah hidung-belakang ROV, sumbu Y dengan kiri-kanan, dan sumbu Z ke atas saat ROV level. Periksa label sumbu pada modul karena orientasi breakout dapat berbeda. Setelah boot, pastikan Pitch/Roll mendekati 0° saat posisi ini dipertahankan. Firmware memakai sampel akselerometer pertama untuk memulai estimasi sudut; firmware belum melakukan kalibrasi bias atau menyimpan offset IMU otomatis.</p>
  <div class="chart-grid">
    <article class="chart-card">
      <div class="chart-heading"><div><h3>Kemiringan ROV</h3><p>Pitch dan roll · derajat</p></div><span class="chart-window">30 dtk</span></div>
      <canvas id="attitudeChart" role="img" aria-label="Grafik histori pitch dan roll, dalam derajat"></canvas>
      <div class="chart-legend"><span><i class="legend-line pitch-line"></i>Pitch</span><span><i class="legend-line roll-line"></i>Roll</span></div>
    </article>
    <article class="chart-card">
      <div class="chart-heading"><div><h3>Laju rotasi</h3><p>Giroskop X, Y, Z · derajat per detik</p></div><span class="chart-window">30 dtk</span></div>
      <canvas id="gyroChart" role="img" aria-label="Grafik histori giroskop sumbu X, Y, dan Z, dalam derajat per detik"></canvas>
      <div class="chart-legend"><span><i class="legend-line gyro-x-line"></i>X</span><span><i class="legend-line gyro-y-line"></i>Y</span><span><i class="legend-line gyro-z-line"></i>Z</span></div>
    </article>
    <article class="chart-card">
      <div class="chart-heading"><div><h3>Tekanan air</h3><p>Tekanan absolut · mbar</p></div><span class="chart-window">30 dtk</span></div>
      <canvas id="pressureChart" role="img" aria-label="Grafik histori tekanan absolut MS5803-14BA dalam milibar"></canvas>
      <div class="chart-legend"><span><i class="legend-line pressure-line"></i>MS5803-14BA</span></div>
    </article>
  </div>
  <p class="chart-note">Nolkan saat ROV diam di permukaan air. Kedalaman adalah perkiraan dari tekanan (asumsi air laut 1025 kg/m³), bukan pengukuran posisi; titik nol hilang saat Pico reboot. Mode Auto harus diaktifkan manual setelah ARM dan dinolkan kembali setiap reboot.</p>
</section>
<section id="panel-control" class="panel" aria-labelledby="tab-control">
  <h2>Kontrol</h2>
  <p class="small">Pilih ARM, lalu tahan tombol gerakan untuk memakai mixer atau gunakan kontrol langsung T1–T6 untuk menguji satu motor. Mode Auto menahan kedalaman dan mengoreksi pitch/roll setelah sensor kedalaman dinolkan. Lepaskan tombol gerakan untuk menetralkan input manual.</p>
  <p><button id="arm" class="danger" type="button">ARM</button><button id="stop" type="button">STOP</button></p>
  <h3>Urutan uji berdurasi</h3>
  <p class="small"><strong>Bukan navigasi otomatis:</strong> tiap tahap berjalan 10 detik, bukan jarak 1 m atau sudut 180° yang terukur. Urutan: maju → putar kanan → mundur → maju → geser kanan → geser kiri. Hasil bergantung pada mixer, arah thruster, daya, dan kondisi air. Pastikan T1–T4 sudah diuji berfungsi. Uji awal tanpa baling-baling, amankan ROV, awasi langsung, dan siapkan STOP/pemutus daya independen.</p>
  <button id="startSequence" type="button" disabled>Mulai urutan (6 × 10 detik)</button>
  <p id="sequenceStatus" class="quick-note" role="status" aria-live="polite">ARM dahulu; urutan tidak berjalan otomatis saat boot.</p>
  <p><button id="stabilization" type="button" aria-pressed="false" disabled>Stabilisasi pitch/roll: OFF</button></p>
  <p id="stabilizationStatus" class="quick-note" aria-live="polite">ARM dan sensor MPU-6050 harus siap untuk menggunakan stabilisasi.</p>
  <p><button id="autoMode" type="button" aria-pressed="false" disabled>Mode Auto kedalaman + stabilisasi: OFF</button></p>
  <p id="autoModeStatus" class="quick-note" aria-live="polite">Nolkan sensor kedalaman di permukaan dan ARM untuk mengaktifkan Auto.</p>
  <p class="small">Stabilisasi menargetkan sudut 0° terhadap gravitasi. Untuk hasil yang tepat, pasang IMU sejajar dengan badan ROV dan lakukan boot dalam posisi level serta diam; verifikasi Pitch/Roll mendekati 0° pada halaman Monitoring. Jika tidak, periksa kerataan ROV dan orientasi sumbu modul sebelum mengoperasikan thruster.</p>
  <h3>Kontrol praktis per gerakan</h3>
  <label class="quick-strength">Kekuatan gerakan <input id="quickStrength" type="range" min="10" max="100" step="5" value="30"><output id="quickStrengthValue">30%</output></label>
  <div class="quick-controls">
    <div class="quick-card"><h3>Maju / mundur</h3><div class="quick-actions">
      <button class="quick-button" type="button" data-axis="0" data-direction="1" aria-label="Tahan untuk maju" aria-pressed="false">Maju</button>
      <button class="quick-button" type="button" data-axis="0" data-direction="-1" aria-label="Tahan untuk mundur" aria-pressed="false">Mundur</button>
    </div></div>
    <div class="quick-card"><h3>Geser kiri / kanan</h3><div class="quick-actions">
      <button class="quick-button" type="button" data-axis="1" data-direction="-1" aria-label="Tahan untuk geser kiri" aria-pressed="false">Kiri</button>
      <button class="quick-button" type="button" data-axis="1" data-direction="1" aria-label="Tahan untuk geser kanan" aria-pressed="false">Kanan</button>
    </div></div>
    <div class="quick-card"><h3>Naik / turun</h3><div class="quick-actions">
      <button class="quick-button" type="button" data-axis="2" data-direction="1" aria-label="Tahan untuk naik" aria-pressed="false">Naik</button>
      <button class="quick-button" type="button" data-axis="2" data-direction="-1" aria-label="Tahan untuk turun" aria-pressed="false">Turun</button>
    </div></div>
    <div class="quick-card"><h3>Putar yaw</h3><div class="quick-actions">
      <button class="quick-button" type="button" data-axis="3" data-direction="-1" aria-label="Tahan untuk putar kiri" aria-pressed="false">Putar kiri</button>
      <button class="quick-button" type="button" data-axis="3" data-direction="1" aria-label="Tahan untuk putar kanan" aria-pressed="false">Putar kanan</button>
    </div></div>
    <div class="quick-card"><h3>Pitch</h3><div class="quick-actions">
      <button class="quick-button" type="button" data-axis="4" data-direction="1" aria-label="Tahan untuk pitch positif" aria-pressed="false">Pitch +</button>
      <button class="quick-button" type="button" data-axis="4" data-direction="-1" aria-label="Tahan untuk pitch negatif" aria-pressed="false">Pitch −</button>
    </div></div>
    <div class="quick-card"><h3>Roll</h3><div class="quick-actions">
      <button class="quick-button" type="button" data-axis="5" data-direction="1" aria-label="Tahan untuk roll positif" aria-pressed="false">Roll +</button>
      <button class="quick-button" type="button" data-axis="5" data-direction="-1" aria-label="Tahan untuk roll negatif" aria-pressed="false">Roll −</button>
    </div></div>
  </div>
  <p id="quickStatus" class="quick-note" aria-live="polite">Thruster harus di-ARM sebelum kontrol gerakan.</p>
  <h3>Kontrol sumbu presisi</h3>
  <div id="axes"></div>
  <h3>Kontrol langsung T1–T6</h3>
  <p class="small">Tahan tombol arah + atau arah − untuk menggerakkan satu thruster saja. Lepaskan tombol untuk kembali netral. Kontrol langsung menonaktifkan mixer sementara; batas daya dan ramp tetap berlaku.</p>
  <p class="small"><strong>Keselamatan:</strong> lepas propeler atau amankan ROV sebelum pengujian. Pastikan thruster yang bergerak sesuai dengan label dan arah yang diharapkan.</p>
  <label class="quick-strength">Kekuatan thruster <input id="individualStrength" type="range" min="10" max="100" step="5" value="20"><output id="individualStrengthValue">20%</output></label>
  <div id="individualControls" class="individual-grid"></div>
  <p id="individualStatus" class="quick-note" aria-live="polite">ARM thruster sebelum kontrol langsung.</p>
</section>
<section id="panel-settings" class="panel" aria-labelledby="tab-settings">
  <h2>Pengaturan</h2>
  <p class="settings-intro">Parameter disimpan ke flash Pico hanya setelah menekan tombol simpan. Perubahan pin dan rentang sinyal diterapkan setelah Pico reboot. DISARM sebelum mengubah parameter.</p>
  <div id="configMessage" class="config-message" role="status" aria-live="polite">
    <span id="configMessageText">Memuat konfigurasi thruster dari Pico...</span>
    <button id="retryConfig" type="button" hidden>Coba muat lagi</button>
  </div>
  <div class="settings-group">
    <h3>Parameter sistem dan koneksi</h3>
    <div class="settings-fields">
      <label>Batas daya maksimum (%) <input id="power" type="number" min="1" max="100"></label>
      <label>Ramp perubahan daya (%/s) <input id="ramp" type="number" min="5" max="200"></label>
      <label>Failsafe kontrol (ms) <input id="failsafe" type="number" min="250" max="2000"></label>
      <label>Nama hotspot Pico (kosongkan untuk tetap) <input id="ssid" maxlength="32" autocomplete="off"></label>
      <label>Password web baru (kosongkan untuk tidak mengubah) <input id="webPass" type="password" minlength="6" maxlength="32" autocomplete="new-password"></label>
    </div>
    <p class="small">Hotspot SAUVAC selalu terbuka tanpa password Wi-Fi.</p>
  </div>
  <div class="settings-group">
    <h3>PID stabilisasi dan kedalaman otomatis</h3>
    <p class="small">Mode Auto memakai target kedalaman dari nol permukaan (cm) dan menjaga pitch/roll. Kp/Ki/Kd, deadband, target, dan batas output dapat diatur di sini. Batas daya maksimum sistem tetap membatasi semua thruster; arah mixer dan thruster vertikal wajib diuji aman sebelum dioperasikan.</p>
    <div class="settings-fields">
      <label>Target kedalaman (cm) <input id="depthTarget" type="number" min="0" max="500" step="1"></label>
      <label>Kedalaman Kp <input id="depthKp" type="number" min="0" max="2" step="0.001"></label>
      <label>Kedalaman Ki <input id="depthKi" type="number" min="0" max="0.5" step="0.001"></label>
      <label>Kedalaman Kd <input id="depthKd" type="number" min="0" max="1" step="0.001"></label>
      <label>Batas output kedalaman (%) <input id="depthMaxOutput" type="number" min="5" max="100" step="1"></label>
      <label>Deadband kedalaman (cm) <input id="depthDeadband" type="number" min="0" max="25" step="1"></label>
      <label>Pitch/roll Kp <input id="stabilizationKp" type="number" min="0" max="1" step="0.001"></label>
      <label>Pitch/roll Ki <input id="stabilizationKi" type="number" min="0" max="0.1" step="0.001"></label>
      <label>Pitch/roll Kd <input id="stabilizationKd" type="number" min="0" max="1" step="0.001"></label>
      <label>Batas output stabilisasi (%) <input id="stabilizationMaxOutput" type="number" min="5" max="100" step="1"></label>
      <label>Batas target sudut (°) <input id="stabilizationMaxAngle" type="number" min="5" max="45" step="1"></label>
      <label>Laju perubahan target (°/s) <input id="stabilizationTargetRate" type="number" min="1" max="60" step="1"></label>
    </div>
    <p class="small">Satuan gain kedalaman: output fraksi per cm, fraksi per cm·detik, dan fraksi per (cm/detik). Satuan gain pitch/roll: output fraksi per derajat, fraksi per derajat·detik, dan fraksi per (°/detik). Atur bertahap dengan ROV diamankan.</p>
  </div>
  <div class="settings-group">
    <h3>Parameter setiap thruster</h3>
    <p class="small">Atur GPIO dan batas pulsa ESC (µs), arah putaran, serta kontribusi mixer setiap motor terhadap sumbu gerak.</p>
    <dl class="mixer-help" aria-label="Arti sumbu mixer">
      <div><dt>Surge</dt><dd>Gerak maju atau mundur ROV.</dd></div>
      <div><dt>Sway</dt><dd>Geser ke kiri atau kanan.</dd></div>
      <div><dt>Heave</dt><dd>Gerak naik atau turun.</dd></div>
      <div><dt>Yaw</dt><dd>Putar hidung ROV ke kiri atau kanan.</dd></div>
      <div><dt>Pitch</dt><dd>Condongkan hidung ke atas atau ke bawah.</dd></div>
      <div><dt>Roll</dt><dd>Miringkan badan ke kiri atau ke kanan.</dd></div>
    </dl>
    <p class="small"><strong>Cara membaca mixer:</strong> setiap baris adalah satu thruster, setiap kolom adalah sumbu gerak. Nilai <code>0</code> berarti motor tidak membantu sumbu itu; nilai positif atau negatif menentukan arah kontribusi, sedangkan besar nilainya menentukan bobotnya. Contoh: <code>1</code> kontribusi penuh, <code>-1</code> arah sebaliknya, <code>0.5</code> setengah, dan <code>0</code> tidak berkontribusi.</p>
    <p class="small"><strong>Catatan:</strong> kontribusi mixer adalah kombinasi kontrol, bukan kalibrasi arah fisik otomatis. Arah aktual dipengaruhi pemasangan dan opsi Balik arah. Enam thruster pada susunan ini tidak memberi kendali roll independen secara bawaan. Uji perubahan kecil dengan DISARM, lalu verifikasi satu per satu dengan propeler dilepas sebelum uji gerak.</p>
  </div>
  <div id="motors"></div>
  <button id="save" disabled>Simpan pengaturan dan reboot</button>
</section>
</main>
</div>
<script>
const axisNames=['Maju/mundur','Geser kiri/kanan','Naik/turun','Yaw','Pitch','Roll'];
const motorNames=['T1 Depan kiri','T2 Depan kanan','T3 Belakang kiri','T4 Belakang kanan','T5 Vertikal depan','T6 Vertikal belakang'];
const testSequenceNames=['Maju','Putar kanan','Mundur','Maju','Geser kanan','Geser kiri'];
let configLoaded=false;
function setConfigMessage(message,isError=false){
  const box=document.getElementById('configMessage');
  box.classList.toggle('error',isError);
  box.setAttribute('role',isError?'alert':'status');
  document.getElementById('configMessageText').textContent=message;
  document.getElementById('retryConfig').hidden=!isError;
}
document.getElementById('retryConfig').addEventListener('click',()=>{
  loadConfig().catch(error=>setConfigMessage(`Konfigurasi gagal dimuat: ${error.message}`,true));
});
const individualControls=document.getElementById('individualControls');
motorNames.forEach((name,i)=>{
  individualControls.insertAdjacentHTML('beforeend',`<article class="individual-card">
    <h4>${name}</h4>
    <output id="individualValue${i}" class="individual-reading">Perintah: 0%</output>
    <div class="individual-actions">
      <button class="individual-button" type="button" data-motor="${i}" data-direction="1" aria-label="Tahan untuk ${name} arah plus" aria-pressed="false">Tahan arah +</button>
      <button class="individual-button" type="button" data-motor="${i}" data-direction="-1" aria-label="Tahan untuk ${name} arah minus" aria-pressed="false">Tahan arah −</button>
    </div>
  </article>`);
});
const axesBox=document.getElementById('axes');
axisNames.forEach((name,i)=>{
  axesBox.insertAdjacentHTML('beforeend',`<label class="axis">${name}<input type="range" min="-100" max="100" value="0" id="a${i}"><span id="v${i}">0</span></label>`);
  document.getElementById('a'+i).addEventListener('input',e=>{
    document.getElementById('v'+i).textContent=e.target.value;
    controlMode='axes';
    releaseAllIndividualControls();
    if(armed) controls();
  });
});
let armed=false;
let controlMode='axes';
let testSequenceActive=false;
let testSequenceStep=0;
let testSequenceHeartbeatInFlight=false;
let testSequenceGeneration=0;
let testSequenceConfirmationPending=false;
let testSequenceConfirmationTimeout=0;
let stabilizationEnabled=false;
let stabilizationFaulted=false;
let stabilizationImuOnline=false;
let stabilizationPitchMixer=false;
let stabilizationRollMixer=false;
let autoModeEnabled=false;
let pressureOnline=false;
let depthCalibrated=false;
let autoHeaveMixer=false;
let individualMotor=0;
let individualValue=0;
let activeIndividualButton=null;
let controlRequestInFlight=false;
let controlNeedsUpdate=false;
function renderStabilizationStatus(){
  const button=document.getElementById('stabilization');
  const status=document.getElementById('stabilizationStatus');
  button.textContent=stabilizationFaulted
    ?'Reset stabilisasi'
    :`Stabilisasi pitch/roll: ${stabilizationEnabled?'ON':'OFF'}`;
  button.setAttribute('aria-pressed',String(stabilizationEnabled));
  button.disabled=testSequenceActive||!armed||controlMode==='individual'||(!stabilizationFaulted&&
    (!stabilizationImuOnline||(!stabilizationPitchMixer&&!stabilizationRollMixer)));
  if(controlMode==='individual'){
    status.textContent='Keluar dari kontrol langsung untuk menggunakan stabilisasi.';
  }else if(!armed){
    status.textContent='ARM dahulu; stabilisasi akan mati saat DISARM atau STOP.';
  }else if(stabilizationFaulted){
    status.textContent='IMU gagal; perintah Pitch/Roll dinetralkan. Reset untuk kembali ke kontrol manual.';
  }else if(!stabilizationImuOnline){
    status.textContent='Sensor MPU-6050 tidak tersedia; stabilisasi tidak dapat diaktifkan.';
  }else if(!stabilizationPitchMixer&&!stabilizationRollMixer){
    status.textContent='Mixer pitch dan roll belum dikonfigurasi.';
  }else if(stabilizationEnabled){
    const unavailable=[
      !stabilizationPitchMixer?'pitch':'',
      !stabilizationRollMixer?'roll':''
    ].filter(Boolean);
    status.textContent=unavailable.length
      ?`Aktif, tetapi sumbu ${unavailable.join(' dan ')} tidak memiliki kontribusi mixer.`
      :'Aktif: tahan tombol Pitch/Roll untuk mengubah target sudut.';
  }else{
    status.textContent='Menjaga sudut 0°; tahan tombol Pitch/Roll untuk mengubah target.';
  }
}
function renderAutoModeStatus(){
  const button=document.getElementById('autoMode');
  const status=document.getElementById('autoModeStatus');
  button.textContent=`Mode Auto kedalaman + stabilisasi: ${autoModeEnabled?'ON':'OFF'}`;
  button.setAttribute('aria-pressed',String(autoModeEnabled));
  const ready=depthCalibrated&&pressureOnline&&stabilizationImuOnline&&
    autoHeaveMixer&&(stabilizationPitchMixer||stabilizationRollMixer);
  button.disabled=testSequenceActive||!armed||controlMode==='individual'||
    (!autoModeEnabled&&!ready);
  document.getElementById('a2').disabled=autoModeEnabled||testSequenceActive;
  document.querySelectorAll('.quick-button[data-axis="2"]').forEach(item=>{
    item.disabled=autoModeEnabled||testSequenceActive;
  });
  if(controlMode==='individual'){
    status.textContent='Keluar dari kontrol langsung sebelum mengaktifkan mode Auto.';
  }else if(!armed){
    status.textContent='ARM dahulu; mode Auto hanya aktif saat kontrol masih terhubung.';
  }else if(autoModeEnabled){
    status.textContent=`Aktif: target ${Number(document.getElementById('depthTarget').value||0)} cm; STOP atau DISARM untuk mematikan.`;
  }else if(!pressureOnline||!depthCalibrated){
    status.textContent='Sensor tekanan harus online dan dinolkan di permukaan.';
  }else if(!stabilizationImuOnline){
    status.textContent='Sensor MPU-6050 belum siap.';
  }else if(!autoHeaveMixer||(!stabilizationPitchMixer&&!stabilizationRollMixer)){
    status.textContent='Konfigurasikan mixer heave dan minimal satu sumbu pitch/roll.';
  }else{
    status.textContent='Siap. Target kedalaman diatur pada Pengaturan; uji arah thruster lebih dahulu.';
  }
}
function renderTestSequenceStatus(){
  const button=document.getElementById('startSequence');
  const status=document.getElementById('sequenceStatus');
  button.disabled=!armed||testSequenceActive;
  button.textContent=testSequenceConfirmationPending
    ?'Konfirmasi mulai (6 × 10 detik)'
    :'Mulai urutan (6 × 10 detik)';
  document.getElementById('arm').disabled=testSequenceActive;
  document.querySelectorAll('.quick-button,.individual-button,#quickStrength,#individualStrength,#axes input')
    .forEach(control=>{control.disabled=testSequenceActive;});
  if(testSequenceActive){
    const step=Math.max(1,Math.min(testSequenceStep,6));
    status.textContent=`Urutan berjalan — tahap ${step}/6: ${testSequenceNames[step-1]} (10 detik). STOP untuk membatalkan.`;
  }else if(testSequenceConfirmationPending){
    status.textContent='Periksa pengamanan ROV, lalu tekan Konfirmasi mulai dalam 10 detik. Tekan STOP untuk membatalkan.';
  }else if(!armed){
    status.textContent='ARM dahulu; urutan tidak berjalan otomatis saat boot.';
  }else{
    status.textContent='Siap. Tekan Mulai untuk urutan 6 tahap; STOP tetap tersedia.';
  }
}
function resetTestSequenceConfirmation(){
  testSequenceConfirmationPending=false;
  if(testSequenceConfirmationTimeout){
    clearTimeout(testSequenceConfirmationTimeout);
    testSequenceConfirmationTimeout=0;
  }
}
function post(path,data){
  return fetch(path,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(data)});
}
function controlFailed(error){
  testSequenceGeneration++;
  resetTestSequenceConfirmation();
  armed=false;
  testSequenceActive=false;
  controlMode='axes';
  releaseAllQuickControls();
  releaseAllIndividualControls();
  document.getElementById('arm').textContent='ARM';
  document.getElementById('arm').setAttribute('aria-pressed','false');
  document.getElementById('status').textContent=`Kontrol gagal: ${error.message}`;
  document.getElementById('quickStatus').textContent='Kontrol dihentikan — periksa koneksi dan status thruster.';
  stabilizationEnabled=false;
  autoModeEnabled=false;
  renderStabilizationStatus();
  renderAutoModeStatus();
  renderTestSequenceStatus();
}
function controls(){
  if(controlRequestInFlight){
    controlNeedsUpdate=true;
    return;
  }
  controlRequestInFlight=true;
  let path='/api/control';
  let data={armed:armed?'1':'0'};
  if(controlMode==='individual'){
    path='/api/thruster/control';
    data={motor:String(individualMotor),value:String(individualValue)};
  }else{
    for(let i=0;i<6;i++) data['a'+i]=document.getElementById('a'+i).value;
  }
  post(path,data).then(async response=>{
    if(!response.ok) throw new Error(await response.text());
  }).catch(controlFailed).finally(()=>{
    controlRequestInFlight=false;
    if(controlNeedsUpdate){
      controlNeedsUpdate=false;
      controls();
    }
  });
}
document.getElementById('arm').onclick=()=>{
  if(!armed&&!confirm('Pastikan thruster aman dan area bebas. Arm thruster?')) return;
  resetTestSequenceConfirmation();
  armed=!armed;document.getElementById('arm').textContent=armed?'DISARM':'ARM';
  document.getElementById('arm').setAttribute('aria-pressed',String(armed));
  if(!armed){
    stabilizationEnabled=false;
    autoModeEnabled=false;
  }
  renderStabilizationStatus();
  renderAutoModeStatus();
  renderTestSequenceStatus();
  controlMode='axes';
  releaseAllQuickControls();
  releaseAllIndividualControls();
  for(let i=0;i<6;i++) setAxisValue(i,0);
  document.getElementById('quickStatus').textContent=armed?'ARMED — tahan tombol untuk bergerak.':'Thruster harus di-ARM sebelum kontrol gerakan.';
  controls();
};
document.getElementById('stop').onclick=()=>{
  testSequenceGeneration++;
  resetTestSequenceConfirmation();
  releaseAllQuickControls();
  releaseAllIndividualControls();
  armed=false;document.getElementById('arm').textContent='ARM';
  document.getElementById('arm').setAttribute('aria-pressed','false');
  testSequenceActive=false;
  testSequenceStep=0;
  stabilizationEnabled=false;
  autoModeEnabled=false;
  renderStabilizationStatus();
  renderAutoModeStatus();
  controlMode='axes';
  document.getElementById('quickStatus').textContent='STOP aktif — semua sumbu netral.';
  for(let i=0;i<6;i++){document.getElementById('a'+i).value=0;document.getElementById('v'+i).textContent='0';}
  controls();
  renderTestSequenceStatus();
};
document.getElementById('startSequence').onclick=async()=>{
  if(!armed||testSequenceActive) return;
  if(!testSequenceConfirmationPending){
    testSequenceConfirmationPending=true;
    renderTestSequenceStatus();
    testSequenceConfirmationTimeout=window.setTimeout(()=>{
      resetTestSequenceConfirmation();
      renderTestSequenceStatus();
    },10000);
    return;
  }
  resetTestSequenceConfirmation();
  const generation=++testSequenceGeneration;
  const button=document.getElementById('startSequence');
  button.disabled=true;
  testSequenceActive=true;
  testSequenceStep=1;
  renderTestSequenceStatus();
  renderStabilizationStatus();
  autoModeEnabled=false;
  renderAutoModeStatus();
  try{
    const response=await post('/api/sequence/start',{strength:quickStrength.value});
    if(!response.ok) throw new Error(await response.text());
    const result=await response.json();
    if(generation!==testSequenceGeneration) return;
    testSequenceActive=Boolean(result.active);
    testSequenceStep=Number(result.step)||0;
    renderTestSequenceStatus();
  }catch(error){
    if(generation!==testSequenceGeneration) return;
    resetTestSequenceConfirmation();
    testSequenceActive=false;
    testSequenceStep=0;
    renderTestSequenceStatus();
    renderStabilizationStatus();
    renderAutoModeStatus();
    document.getElementById('sequenceStatus').textContent=`Urutan tidak dimulai: ${error.message}`;
  }
};
document.getElementById('stabilization').onclick=async()=>{
  const button=document.getElementById('stabilization');
  const requestedEnabled=stabilizationFaulted?false:!stabilizationEnabled;
  button.disabled=true;
  try{
    const response=await post('/api/stabilization',{enabled:requestedEnabled?'1':'0'});
    if(!response.ok) throw new Error(await response.text());
    const result=await response.json();
    stabilizationEnabled=Boolean(result.enabled);
    stabilizationFaulted=Boolean(result.faulted);
    if(!result.enabled) autoModeEnabled=false;
    renderStabilizationStatus();
    renderAutoModeStatus();
  }catch(error){
    renderStabilizationStatus();
    renderAutoModeStatus();
    document.getElementById('stabilizationStatus').textContent=
      `Gagal mengubah stabilisasi: ${error.message}`;
  }
};
document.getElementById('autoMode').onclick=async()=>{
  const button=document.getElementById('autoMode');
  const requestedEnabled=!autoModeEnabled;
  button.disabled=true;
  if(requestedEnabled){
    releaseAllQuickControls();
    for(let i=0;i<6;i++) setAxisValue(i,0);
  }
  try{
    const response=await post('/api/auto',{enabled:requestedEnabled?'1':'0'});
    if(!response.ok) throw new Error(await response.text());
    const result=await response.json();
    autoModeEnabled=Boolean(result.enabled);
    stabilizationEnabled=autoModeEnabled;
    renderStabilizationStatus();
    renderAutoModeStatus();
    if(armed) controls();
  }catch(error){
    renderAutoModeStatus();
    document.getElementById('autoModeStatus').textContent=
      `Gagal mengubah mode Auto: ${error.message}`;
  }
};
const activeQuickAxes=new Map();
const quickButtons=[...document.querySelectorAll('.quick-button')];
const quickStrength=document.getElementById('quickStrength');
const individualStrength=document.getElementById('individualStrength');
const individualButtons=[...document.querySelectorAll('.individual-button')];
function clearIndividualButton(){
  if(activeIndividualButton) activeIndividualButton.setAttribute('aria-pressed','false');
  activeIndividualButton=null;
  individualValue=0;
  document.getElementById('individualValue'+individualMotor).textContent='Perintah: 0%';
}
function releaseAllIndividualControls(){
  clearIndividualButton();
}
function setAxisValue(axis,value){
  const slider=document.getElementById('a'+axis);
  slider.value=String(value);
  document.getElementById('v'+axis).textContent=String(value);
}
function releaseQuickControl(button){
  const axis=Number(button.dataset.axis);
  if(activeQuickAxes.get(axis)!==button) return;
  activeQuickAxes.delete(axis);
  button.setAttribute('aria-pressed','false');
  setAxisValue(axis,0);
  document.getElementById('quickStatus').textContent='Gerakan dilepas — sumbu kembali netral.';
  if(armed) controls();
}
function releaseAllQuickControls(){
  for(const [axis,button] of activeQuickAxes){
    button.setAttribute('aria-pressed','false');
    setAxisValue(axis,0);
  }
  activeQuickAxes.clear();
}
function startQuickControl(button){
  if(!armed){
    document.getElementById('quickStatus').textContent='Tekan ARM terlebih dahulu. Gerakan tidak dikirim.';
    return false;
  }
  const axis=Number(button.dataset.axis);
  if(activeQuickAxes.has(axis)) return false;
  clearIndividualButton();
  controlMode='axes';
  activeQuickAxes.set(axis,button);
  button.setAttribute('aria-pressed','true');
  setAxisValue(axis,Number(button.dataset.direction)*Number(quickStrength.value));
  document.getElementById('quickStatus').textContent=`Menjalankan: ${button.getAttribute('aria-label').replace('Tahan untuk ','')}. Lepaskan untuk netral.`;
  controls();
  return true;
}
function startIndividualControl(button){
  if(!armed){
    document.getElementById('individualStatus').textContent='Tekan ARM terlebih dahulu. Thruster tidak digerakkan.';
    return false;
  }
  releaseAllQuickControls();
  for(let i=0;i<6;i++) setAxisValue(i,0);
  if(activeIndividualButton) activeIndividualButton.setAttribute('aria-pressed','false');
  activeIndividualButton=button;
  individualMotor=Number(button.dataset.motor);
  individualValue=Number(button.dataset.direction)*Number(individualStrength.value);
  controlMode='individual';
  button.setAttribute('aria-pressed','true');
  document.getElementById('individualValue'+individualMotor).textContent=`Perintah: ${individualValue}%`;
  document.getElementById('individualStatus').textContent=`Menggerakkan ${motorNames[individualMotor]} saja. Lepaskan untuk netral.`;
  stabilizationEnabled=false;
  autoModeEnabled=false;
  renderStabilizationStatus();
  renderAutoModeStatus();
  controls();
  return true;
}
function releaseIndividualControl(button){
  if(activeIndividualButton!==button) return;
  clearIndividualButton();
  controlMode='axes';
  document.getElementById('individualStatus').textContent='Thruster dilepas — perintah kembali netral.';
  renderStabilizationStatus();
  renderAutoModeStatus();
  if(armed) controls();
}
document.getElementById('quickStrengthValue').textContent=quickStrength.value+'%';
quickStrength.addEventListener('input',()=>{
  document.getElementById('quickStrengthValue').textContent=quickStrength.value+'%';
  for(const [axis,button] of activeQuickAxes){
    setAxisValue(axis,Number(button.dataset.direction)*Number(quickStrength.value));
  }
  if(activeQuickAxes.size) controls();
});
document.getElementById('individualStrengthValue').textContent=individualStrength.value+'%';
individualStrength.addEventListener('input',()=>{
  document.getElementById('individualStrengthValue').textContent=individualStrength.value+'%';
  if(activeIndividualButton){
    individualValue=Number(activeIndividualButton.dataset.direction)*Number(individualStrength.value);
    document.getElementById('individualValue'+individualMotor).textContent=`Perintah: ${individualValue}%`;
    controls();
  }
});
individualButtons.forEach(button=>{
  button.addEventListener('pointerdown',event=>{
    event.preventDefault();
    if(startIndividualControl(button)) button.setPointerCapture(event.pointerId);
  });
  button.addEventListener('pointerup',()=>releaseIndividualControl(button));
  button.addEventListener('pointercancel',()=>releaseIndividualControl(button));
  button.addEventListener('lostpointercapture',()=>releaseIndividualControl(button));
  button.addEventListener('keydown',event=>{
    if((event.key===' '||event.key==='Enter')&&!event.repeat){
      event.preventDefault();
      startIndividualControl(button);
    }
  });
  button.addEventListener('keyup',event=>{
    if(event.key===' '||event.key==='Enter'){
      event.preventDefault();
      releaseIndividualControl(button);
    }
  });
  button.addEventListener('blur',()=>releaseIndividualControl(button));
});
quickButtons.forEach(button=>{
  button.addEventListener('pointerdown',event=>{
    event.preventDefault();
    if(startQuickControl(button)) button.setPointerCapture(event.pointerId);
  });
  button.addEventListener('pointerup',()=>releaseQuickControl(button));
  button.addEventListener('pointercancel',()=>releaseQuickControl(button));
  button.addEventListener('lostpointercapture',()=>releaseQuickControl(button));
  button.addEventListener('keydown',event=>{
    if((event.key===' '||event.key==='Enter')&&!event.repeat){
      event.preventDefault();
      startQuickControl(button);
    }
  });
  button.addEventListener('keyup',event=>{
    if(event.key===' '||event.key==='Enter'){
      event.preventDefault();
      releaseQuickControl(button);
    }
  });
  button.addEventListener('blur',()=>releaseQuickControl(button));
});
window.addEventListener('blur',()=>{
  if(testSequenceConfirmationPending){
    resetTestSequenceConfirmation();
    renderTestSequenceStatus();
  }
  if(!activeQuickAxes.size&&!activeIndividualButton&&!testSequenceActive) return;
  testSequenceGeneration++;
  releaseAllQuickControls();
  releaseAllIndividualControls();
  armed=false;
  testSequenceActive=false;
  testSequenceStep=0;
  controlMode='axes';
  document.getElementById('arm').textContent='ARM';
  document.getElementById('arm').setAttribute('aria-pressed','false');
  document.getElementById('quickStatus').textContent='Halaman kehilangan fokus — thruster di-DISARM.';
  controls();
  renderTestSequenceStatus();
  renderStabilizationStatus();
});
document.addEventListener('visibilitychange',()=>{
  if(document.hidden&&armed){
    testSequenceGeneration++;
    resetTestSequenceConfirmation();
    releaseAllQuickControls();
    releaseAllIndividualControls();
    armed=false;
    testSequenceActive=false;
    testSequenceStep=0;
    controlMode='axes';
    document.getElementById('arm').textContent='ARM';
    document.getElementById('arm').setAttribute('aria-pressed','false');
    document.getElementById('quickStatus').textContent='Halaman tidak terlihat — thruster di-DISARM.';
    controls();
    renderTestSequenceStatus();
    renderStabilizationStatus();
  }
});
async function sendTestSequenceHeartbeat(){
  if(testSequenceHeartbeatInFlight) return;
  const generation=testSequenceGeneration;
  testSequenceHeartbeatInFlight=true;
  try{
    const response=await post('/api/sequence/heartbeat',{});
    if(!response.ok) throw new Error(await response.text());
    const result=await response.json();
    if(generation!==testSequenceGeneration) return;
    testSequenceActive=Boolean(result.active);
    testSequenceStep=Number(result.step)||0;
    armed=Boolean(result.armed);
    document.getElementById('arm').textContent=armed?'DISARM':'ARM';
    document.getElementById('arm').setAttribute('aria-pressed',String(armed));
    if(!testSequenceActive){
      for(let i=0;i<6;i++) setAxisValue(i,0);
    }
    renderTestSequenceStatus();
    renderStabilizationStatus();
  }catch(error){
    if(generation!==testSequenceGeneration) return;
    controlFailed(error);
    document.getElementById('sequenceStatus').textContent=
      `Koneksi urutan terputus: ${error.message}. Failsafe akan menetralkan thruster.`;
  }finally{
    testSequenceHeartbeatInFlight=false;
  }
}
setInterval(()=>{
  if(testSequenceActive) sendTestSequenceHeartbeat();
  else if(armed) controls();
},150);
async function loadConfig(){
  const retryButton=document.getElementById('retryConfig');
  retryButton.disabled=true;
  setConfigMessage('Memuat konfigurasi thruster dari Pico...');
  try{
    const response=await fetch('/api/config');
    if(!response.ok) throw new Error('Tidak dapat membaca konfigurasi. Pastikan Pico aktif dan login web masih berlaku.');
    const c=await response.json();
    if(!Array.isArray(c.pins)||c.pins.length!==6||!Array.isArray(c.min)||
       !Array.isArray(c.neutral)||!Array.isArray(c.max)||!Array.isArray(c.reverse)||
       !Array.isArray(c.mix)||c.mix.length!==6){
      throw new Error('Format konfigurasi thruster dari Pico tidak lengkap.');
    }
    document.getElementById('power').value=c.power;
    document.getElementById('ramp').value=c.ramp;
    document.getElementById('failsafe').value=c.failsafe;
    document.getElementById('depthTarget').value=c.depthTarget;
    document.getElementById('depthKp').value=c.depthKp;
    document.getElementById('depthKi').value=c.depthKi;
    document.getElementById('depthKd').value=c.depthKd;
    document.getElementById('depthMaxOutput').value=c.depthMaxOutput;
    document.getElementById('depthDeadband').value=c.depthDeadband;
    document.getElementById('stabilizationKp').value=c.stabilizationKp;
    document.getElementById('stabilizationKi').value=c.stabilizationKi;
    document.getElementById('stabilizationKd').value=c.stabilizationKd;
    document.getElementById('stabilizationMaxOutput').value=c.stabilizationMaxOutput;
    document.getElementById('stabilizationMaxAngle').value=c.stabilizationMaxAngle;
    document.getElementById('stabilizationTargetRate').value=c.stabilizationTargetRate;
    document.getElementById('ssid').placeholder=c.ssid||'Belum disetel';
    const box=document.getElementById('motors');
    box.replaceChildren();
    c.pins.forEach((pin,i)=>{
      let html=`<article class="motor"><h4>${motorNames[i]}</h4>
        <div class="motor-fields">
        <label>GPIO output <select id="pin${i}"></select></label>
        <label>Pulsa minimum (µs) <input id="min${i}" type="number" min="900" max="1500"></label>
        <label>Pulsa netral (µs) <input id="neutral${i}" type="number" min="1400" max="1600"></label>
        <label>Pulsa maksimum (µs) <input id="max${i}" type="number" min="1500" max="2100"></label>
        <label class="reverse-label"><input id="reverse${i}" type="checkbox"> Balik arah putaran</label>
        </div><div class="matrix-wrap"><div class="matrix"><b>Thruster</b><b>Surge</b><b>Sway</b><b>Heave</b><b>Yaw</b><b>Pitch</b><b>Roll</b>`;
      html+=`<b>${motorNames[i]}</b>`;
      for(let a=0;a<6;a++) html+=`<input id="m${i}_${a}" type="number" min="-1" max="1" step="0.05" aria-label="${motorNames[i]} mixer ${axisNames[a]}">`;
      html+='</div></div></article>';
      box.insertAdjacentHTML('beforeend',html);
      const select=document.getElementById('pin'+i);
      [...Array(21)].forEach((_,n)=>{const p=n+2;select.add(new Option('GP'+p,p));});
      select.add(new Option('GP28',28));
      select.value=pin;
      document.getElementById('min'+i).value=c.min[i];
      document.getElementById('neutral'+i).value=c.neutral[i];
      document.getElementById('max'+i).value=c.max[i];
      document.getElementById('reverse'+i).checked=!!c.reverse[i];
      for(let a=0;a<6;a++) document.getElementById(`m${i}_${a}`).value=c.mix[i][a];
    });
    const statusResponse=await fetch('/api/status');
    if(!statusResponse.ok) throw new Error('Konfigurasi dimuat, tetapi status Pico tidak dapat dibaca.');
    const status=await statusResponse.json();
    document.getElementById('status').textContent=`Wi-Fi: ${status.ip} | ${status.armed?'ARMED':'DISARMED'} | Batas daya ${c.power}%`;
    configLoaded=true;
    document.getElementById('save').disabled=false;
    setConfigMessage('Konfigurasi T1–T6 berhasil dimuat dari Pico.');
  }catch(error){
    configLoaded=false;
    document.getElementById('save').disabled=true;
    setConfigMessage(`Konfigurasi gagal dimuat: ${error.message}`,true);
    throw error;
  }finally{
    retryButton.disabled=false;
  }
}
const navItems=[...document.querySelectorAll('.nav-link')];
const menuToggle=document.getElementById('menuToggle');
const mobileLayout=window.matchMedia('(max-width:760px)');
function syncMenuButton(){
  const sidebar=document.getElementById('sidebar');
  const expanded=mobileLayout.matches
    ?document.body.classList.contains('sidebar-open')
    :!document.body.classList.contains('sidebar-collapsed');
  if(mobileLayout.matches){
    sidebar.inert=!expanded;
    sidebar.setAttribute('aria-hidden',String(!expanded));
  }else{
    sidebar.inert=false;
    sidebar.removeAttribute('aria-hidden');
  }
  menuToggle.setAttribute('aria-expanded',String(expanded));
  menuToggle.setAttribute('aria-label',expanded
    ?(mobileLayout.matches?'Tutup menu navigasi':'Ciutkan navigasi')
    :'Buka menu navigasi');
}
function closeMobileMenu(){
  document.body.classList.remove('sidebar-open');
  syncMenuButton();
  menuToggle.focus();
}
function selectSection(button){
  navItems.forEach(item=>{
    if(item===button) item.setAttribute('aria-current','page');
    else item.removeAttribute('aria-current');
  });
  document.querySelectorAll('.panel').forEach(panel=>{
    panel.classList.toggle('active',panel.id==='panel-'+button.dataset.panel);
  });
  const page={
    monitor:['Monitoring','SYSTEM OVERVIEW','Status sensor dan telemetri SAUVAC.'],
    control:['Kontrol','PILOT CONSOLE','Kendali thruster SAUVAC.'],
    settings:['Pengaturan','CONFIGURATION','Konfigurasi thruster dan sistem.']
  }[button.dataset.panel];
  document.getElementById('pageTitle').textContent=page[0];
  document.getElementById('pageEyebrow').textContent=page[1];
  document.getElementById('pageDescription').textContent=page[2];
  if(mobileLayout.matches) closeMobileMenu();
}
navItems.forEach(button=>button.addEventListener('click',()=>selectSection(button)));
menuToggle.addEventListener('click',()=>{
  if(mobileLayout.matches){
    const opening=!document.body.classList.contains('sidebar-open');
    document.body.classList.toggle('sidebar-open',opening);
    syncMenuButton();
    if(opening){
      navItems.find(item=>item.hasAttribute('aria-current'))?.focus();
    }
  }else{
    document.body.classList.toggle('sidebar-collapsed');
    syncMenuButton();
  }
});
document.getElementById('sidebarBackdrop').addEventListener('click',closeMobileMenu);
document.addEventListener('keydown',event=>{
  if(event.key==='Escape'&&document.body.classList.contains('sidebar-open')){
    closeMobileMenu();
    menuToggle.focus();
  }
});
mobileLayout.addEventListener('change',()=>{
  document.body.classList.remove('sidebar-open','sidebar-collapsed');
  syncMenuButton();
});
syncMenuButton();
function formatVector(values){return values.map(value=>Number(value).toFixed(2)).join(' / ');}
const attitudeHistory=[[],[]];
const gyroHistory=[[],[],[]];
const pressureHistory=[[]];
const chartColors=['#2878c7','#198754','#d97706','#087e9a'];
function addChartSample(history,values){
  values.forEach((value,index)=>{
    history[index].push(Number.isFinite(value)?value:null);
    if(history[index].length>60) history[index].shift();
  });
}
function drawChart(canvasId,history,minimum,maximum,ticks,colors){
  const canvas=document.getElementById(canvasId);
  const rect=canvas.getBoundingClientRect();
  if(rect.width<1||rect.height<1) return;
  const ratio=Math.min(window.devicePixelRatio||1,2);
  canvas.width=Math.round(rect.width*ratio);
  canvas.height=Math.round(rect.height*ratio);
  const ctx=canvas.getContext('2d');
  ctx.setTransform(ratio,0,0,ratio,0,0);
  const width=rect.width,height=rect.height;
  const plot={left:38,right:8,top:8,bottom:22};
  const plotWidth=width-plot.left-plot.right;
  const plotHeight=height-plot.top-plot.bottom;
  ctx.clearRect(0,0,width,height);
  ctx.font='11px system-ui,sans-serif';
  ctx.textAlign='right';
  ctx.textBaseline='middle';
  ticks.forEach(tick=>{
    const y=plot.top+(maximum-tick)/(maximum-minimum)*plotHeight;
    ctx.beginPath();ctx.strokeStyle='#e6edf2';ctx.lineWidth=1;
    ctx.moveTo(plot.left,y);ctx.lineTo(width-plot.right,y);ctx.stroke();
    ctx.fillStyle='#526678';ctx.fillText(String(tick),plot.left-7,y);
  });
  for(let i=0;i<4;i++){
    const x=plot.left+i*plotWidth/3;
    ctx.beginPath();ctx.strokeStyle='#f0f3f6';ctx.moveTo(x,plot.top);ctx.lineTo(x,height-plot.bottom);ctx.stroke();
  }
  history.forEach((series,seriesIndex)=>{
    ctx.beginPath();ctx.strokeStyle=colors[seriesIndex];ctx.lineWidth=2;ctx.lineJoin='round';
    let drawing=false;
    series.forEach((value,index)=>{
      if(value===null||!Number.isFinite(value)){drawing=false;return;}
      const x=plot.left+(series.length<2?1:index/(series.length-1))*plotWidth;
      const y=plot.top+(maximum-Math.max(minimum,Math.min(maximum,value)))/(maximum-minimum)*plotHeight;
      if(drawing) ctx.lineTo(x,y); else {ctx.moveTo(x,y);drawing=true;}
    });
    ctx.stroke();
  });
  ctx.fillStyle='#65798b';ctx.textAlign='left';ctx.textBaseline='bottom';ctx.fillText('lama',plot.left,height-2);
  ctx.textAlign='right';ctx.fillText('sekarang',width-plot.right,height-2);
}
function drawTelemetryCharts(){
  drawChart('attitudeChart',attitudeHistory,-90,90,[-90,0,90],chartColors);
  drawChart('gyroChart',gyroHistory,-250,250,[-250,0,250],[chartColors[2],chartColors[1],chartColors[3]]);
  const pressureSamples=pressureHistory[0].filter(Number.isFinite);
  if(pressureSamples.length){
    const minimum=Math.min(...pressureSamples);
    const maximum=Math.max(...pressureSamples);
    const padding=Math.max((maximum-minimum)*0.2,0.5);
    const lower=Math.floor((minimum-padding)*10)/10;
    const upper=Math.ceil((maximum+padding)*10)/10;
    drawChart('pressureChart',pressureHistory,lower,upper,
      [lower,(lower+upper)/2,upper],[chartColors[0]]);
  }else{
    drawChart('pressureChart',pressureHistory,900,1100,[900,1000,1100],[chartColors[0]]);
  }
}
window.addEventListener('resize',drawTelemetryCharts);
async function loadTelemetry(){
  try{
    const response=await fetch('/api/telemetry');
    if(!response.ok) throw new Error('Telemetri tidak tersedia');
    const t=await response.json();
    stabilizationEnabled=Boolean(t.stabilization&&t.stabilization.enabled);
    stabilizationFaulted=Boolean(t.stabilization&&t.stabilization.faulted);
    stabilizationImuOnline=Boolean(t.imu.online);
    stabilizationPitchMixer=Boolean(t.stabilization&&t.stabilization.pitchMixer);
    stabilizationRollMixer=Boolean(t.stabilization&&t.stabilization.rollMixer);
    autoModeEnabled=Boolean(t.auto&&t.auto.enabled);
    pressureOnline=Boolean(t.pressure.online);
    depthCalibrated=Boolean(t.pressure.depthCalibrated);
    autoHeaveMixer=Boolean(t.stabilization&&t.stabilization.heaveMixer);
    const sequenceWasActive=testSequenceActive;
    testSequenceActive=Boolean(t.sequence&&t.sequence.active);
    testSequenceStep=Number(t.sequence&&t.sequence.step)||0;
    if(sequenceWasActive&&!testSequenceActive){
      testSequenceGeneration++;
      armed=Boolean(t.armed);
      document.getElementById('arm').textContent=armed?'DISARM':'ARM';
      document.getElementById('arm').setAttribute('aria-pressed',String(armed));
    }
    renderTestSequenceStatus();
    if(stabilizationFaulted){
      setAxisValue(4,0);
      setAxisValue(5,0);
    }
    renderStabilizationStatus();
    renderAutoModeStatus();
    const imu=document.getElementById('imuState');
    imu.textContent=t.imu.online?'Terhubung':'Tidak terdeteksi';
    imu.className=t.imu.online?'good':'bad';
    const compass=document.getElementById('compassState');
    compass.textContent=t.compass.online?t.compass.type:'Tidak terdeteksi';
    compass.className=t.compass.online?'good':'bad';
    document.getElementById('heading').textContent=t.compass.online?`${Number(t.compass.heading).toFixed(1)}°`:'--°';
    document.getElementById('attitude').textContent=t.imu.online?`${Number(t.imu.pitch).toFixed(1)}° / ${Number(t.imu.roll).toFixed(1)}°`:'--° / --°';
    document.getElementById('magnetic').textContent=t.compass.online?formatVector(t.compass.raw):'-- / -- / --';
    document.getElementById('acceleration').textContent=t.imu.online?formatVector(t.imu.accel):'-- / -- / --';
    document.getElementById('rotation').textContent=t.imu.online?formatVector(t.imu.gyro):'-- / -- / --';
    const pressure=document.getElementById('pressureState');
    pressure.textContent=t.pressure.online
      ?`${Number(t.pressure.pressureMbar).toFixed(1)} mbar · ${Number(t.pressure.temperatureC).toFixed(2)} °C`
      :'Tidak terdeteksi';
    pressure.className=t.pressure.online?'good':'bad';
    const vsys=document.getElementById('vsysValue');
    vsys.textContent=t.power&&t.power.vsysOnline
      ?`${Number(t.power.vsysV).toFixed(2)} V`
      :'-- V';
    vsys.className=t.power&&t.power.vsysOnline?'good':'bad';
    document.getElementById('vsysStatus').textContent=t.power&&t.power.vsysOnline
      ?'Tegangan suplai VSYS Pico · bukan estimasi kapasitas baterai.'
      :'Pembacaan VSYS tidak tersedia.';
    document.getElementById('depthValue').textContent=
      t.pressure.online&&t.pressure.depthCalibrated
        ?`${Number(t.pressure.depthM).toFixed(2)} m`
        :'-- m';
    document.getElementById('depthStatus').textContent=!t.pressure.online
      ?'Sensor tekanan tidak tersedia.'
      :t.pressure.depthCalibrated
        ?'Relatif terhadap tekanan saat nol permukaan.'
        :'Nolkan saat ROV diam di permukaan air.';
    document.getElementById('depthZero').disabled=!t.pressure.online||t.armed;
    addChartSample(attitudeHistory,t.imu.online?[Number(t.imu.pitch),Number(t.imu.roll)]:[NaN,NaN]);
    addChartSample(gyroHistory,t.imu.online?t.imu.gyro.map(Number):[NaN,NaN,NaN]);
    addChartSample(pressureHistory,t.pressure.online?[Number(t.pressure.pressureMbar)]:[NaN]);
    drawTelemetryCharts();
    document.getElementById('systemState').textContent=`${t.armed?'ARMED':'DISARMED'} · ${t.thrustersReady?'Thruster siap':'Thruster error'} · I2C ${t.i2cReady?'OK':'ERROR'} · Tekanan ${t.pressure.online?'OK':'ERROR'} · ${t.uptime}s`;
    document.getElementById('systemState').className=t.thrustersReady&&t.i2cReady?'good':'bad';
  }catch(error){
    document.getElementById('systemState').textContent='Koneksi telemetri terputus';
    document.getElementById('systemState').className='bad';
  }
}
document.getElementById('depthZero').addEventListener('click',async()=>{
  const button=document.getElementById('depthZero');
  const status=document.getElementById('depthStatus');
  if(button.disabled) return;
  button.disabled=true;
  status.textContent='Menyimpan nol tekanan permukaan...';
  try{
    const response=await post('/api/depth/zero',{});
    if(!response.ok) throw new Error(await response.text());
    status.textContent='Nol kedalaman disetel. Siap memantau perubahan tekanan.';
    await loadTelemetry();
  }catch(error){
    status.textContent=`Gagal mengatur nol: ${error.message}`;
    button.disabled=false;
  }
});
setInterval(loadTelemetry,500);
loadTelemetry();
document.getElementById('save').onclick=async()=>{
  if(!configLoaded){setConfigMessage('Pengaturan belum dimuat dari Pico. Tekan "Coba muat lagi" sebelum menyimpan.',true);return;}
  if(armed){alert('DISARM thruster sebelum menyimpan.');return;}
  if(!confirm('Simpan semua parameter? Pico akan reboot.')) return;
  const button=document.getElementById('save');button.disabled=true;
  try{
    for(let i=0;i<6;i++){
      const d={motor:i,pin:document.getElementById('pin'+i).value,
        min:document.getElementById('min'+i).value,
        neutral:document.getElementById('neutral'+i).value,
        max:document.getElementById('max'+i).value,
        reverse:document.getElementById('reverse'+i).checked?'1':'0'};
      for(let a=0;a<6;a++) d['mix'+a]=document.getElementById(`m${i}_${a}`).value;
      const r=await post('/api/motor',d);if(!r.ok) throw new Error(await r.text());
    }
    const r=await post('/api/system',{power:document.getElementById('power').value,
      ramp:document.getElementById('ramp').value,
      failsafe:document.getElementById('failsafe').value,
      stabilizationKp:document.getElementById('stabilizationKp').value,
      stabilizationKi:document.getElementById('stabilizationKi').value,
      stabilizationKd:document.getElementById('stabilizationKd').value,
      stabilizationMaxOutput:document.getElementById('stabilizationMaxOutput').value,
      stabilizationMaxAngle:document.getElementById('stabilizationMaxAngle').value,
      stabilizationTargetRate:document.getElementById('stabilizationTargetRate').value,
      depthKp:document.getElementById('depthKp').value,
      depthKi:document.getElementById('depthKi').value,
      depthKd:document.getElementById('depthKd').value,
      depthMaxOutput:document.getElementById('depthMaxOutput').value,
      depthDeadband:document.getElementById('depthDeadband').value,
      depthTarget:document.getElementById('depthTarget').value,
      ssid:document.getElementById('ssid').value,
      webPass:document.getElementById('webPass').value});
    if(!r.ok) throw new Error(await r.text());
    const saved=await post('/api/save',{});
    if(!saved.ok) throw new Error(await saved.text());
    document.getElementById('status').textContent='Tersimpan. Menunggu Pico reboot...';
  }catch(e){alert('Gagal menyimpan: '+e.message);button.disabled=false;}
};
loadConfig().catch(e=>{document.getElementById('status').textContent=e.message;});
</script></body></html>
)HTML";

const char SETUP_PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="id"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Setup ROV Pico W</title>
<style>body{font:16px/1.5 system-ui,-apple-system,"Segoe UI",sans-serif;max-width:600px;margin:30px auto;padding:16px;background:#f4f7fb;color:#172b3a}
section{background:#fff;padding:22px;border:1px solid #dce4ec;border-radius:12px;box-shadow:0 2px 8px #18344b08}
input,button{font:inherit;padding:10px;margin:6px 0;width:100%;box-sizing:border-box;border:1px solid #c7d3de;border-radius:7px;background:#fff;color:#172b3a}
button{min-height:44px;background:#087e9a;border-color:#087e9a;color:#fff;cursor:pointer}
input:focus-visible,button:focus-visible{outline:2px solid #087e9a;outline-offset:2px}</style>
<body><section><h1>Setup hotspot ROV</h1>
<p>Pico W membuat hotspot terbuka tanpa password Wi-Fi di 192.168.4.1. Buat password web unik (minimal 6 karakter) untuk menyelesaikan setup.</p>
<form method="post" action="/setup">
<label>Nama hotspot Pico<input name="ssid" value="SAUVAC" maxlength="32" required></label>
<label>Password web kontrol<input name="webPass" type="password" minlength="6" maxlength="32" autocomplete="new-password" required></label>
<button type="submit">Simpan dan reboot</button></form></section></body></html>
)HTML";

}  // namespace ROVWebPages
