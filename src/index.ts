import path from 'path';

export const include = `"${__dirname}"/../src`;
export const gyp = path.join(__dirname, '/../binding.gyp:napi_thread_safe_promise');
