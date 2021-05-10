import path from 'path';

export const include = `"${path.relative('.',__dirname + "/../src")}"`;
export const gyp = path.join(include, '/../binding.gyp:napi_thread_safe_promise');
