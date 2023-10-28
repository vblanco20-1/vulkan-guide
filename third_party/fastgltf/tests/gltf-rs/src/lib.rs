use base64::Engine;

#[cxx::bridge(namespace = "rust::gltf")]
mod ffi {
    extern "Rust" {
        fn run(data: &[u8]) -> i32;
    }
}

fn run(data: &[u8]) -> i32 {
    // TODO: Decode URIs and data URIs?
    let gltf = gltf::Gltf::from_slice(data)
        .unwrap();

    // Decode URIs
    let json = gltf.document.into_json();

    let mut uri_count = 0;
    for x in json.buffers {
        // gltf-rs doesn't automatically decode base64. Using its "import" feature won't work,
        // because we're not interested in file-loaded buffer/image data.
        if x.uri.is_some() {
            let uri = x.uri.unwrap();
            if let Some(rest) = uri.strip_prefix("data:") {
                let mut it = rest.split(";base64,");
                let data = match (it.next(), it.next()) {
                    (_, Some(data)) => Some(data),
                    (Some(data), _) => Some(data),
                    _ => None
                };
                if data.is_none() {
                    continue;
                }

                base64::engine::GeneralPurpose::new(
                    &base64::alphabet::STANDARD,
                    base64::engine::general_purpose::PAD)
                    .decode(data.unwrap()).expect("Decoded bytes");
                uri_count += 1;
            }
        }
    }

    uri_count
}
